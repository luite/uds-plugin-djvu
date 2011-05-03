#include "pdf_doc.h"
#include "log.h"

#include <string.h> // for memset
#include <ctype.h>
#include <math.h>
#include <vector>

#include "ipc.h"
#include "menu.h"

GlobalParams *globalParams;	

GlobalParams::GlobalParams() {
	WARNPRINTF("Initializing global params");
	ctx = ddjvu_context_create("");
	mtx = new pdf::Mutex();
	if(!ctx) ERRORPRINTF("Error creating DjVu context");
	
	// test for menu!
	ipc_set_services ();
	const char* group = "uds_view"; // "djvu_options";
	ipc_menu_add_group(group, "", "options");
	ipc_menu_add_item("open", group, "open");
	ipc_menu_add_item("copy", group, "copy");
	ipc_menu_set_item_state("copy", group, "disabled");
	// end test
}

GlobalParams::~GlobalParams() {	
	WARNPRINTF("Destructing global params");
	ddjvu_context_release(ctx);
	delete mtx;
}

void handleDdjvu(int wait) {
	ddjvu_context_t *ctx = globalParams->getContext();
	const ddjvu_message_t *msg;
	if (wait) msg = ddjvu_message_wait(ctx);
	while ((msg = ddjvu_message_peek(ctx))) {
	switch(msg->m_any.tag) {
        case DDJVU_ERROR:
		  WARNPRINTF("DjVu Error: %s\n", msg->m_error.message);
          if (msg->m_error.filename) WARNPRINTF("DjVu error in file: '%s:%d'\n",msg->m_error.filename, msg->m_error.lineno);
//          exit(10);
        default:
          break;
      }
      ddjvu_message_pop(ctx);
    }
}

bool endsWith(const char *a, const char *b) {
	if(strlen(a) < strlen(b)) return false;
	return strcmp (a+strlen(a)-strlen(b),b) == 0;
}

OutlineItem* buildOutlineRec(miniexp_t exp) {
	//WARNPRINTF("Building outline - rec");
	if(!miniexp_consp(exp)) return 0;
	int n = miniexp_length(exp);
	//WARNPRINTF("We have list");
	if(n < 2) return 0;
	if(miniexp_stringp(miniexp_nth(0,exp)) &&
		miniexp_stringp(miniexp_nth(1,exp))) {
		const char* txt = miniexp_to_str(miniexp_nth(0,exp));
		const char* dest = miniexp_to_str(miniexp_nth(1,exp));
		//WARNPRINTF("Found outline item: %s (%s)", txt, dest);
		int destpg = 1;
		if(strlen(dest) > 0 && dest[0] == '#') {
			bool allnum = true;
			for(int i=1;i<(int)strlen(dest);i++) if(!isdigit(dest[i])) { allnum = false; break; }
			if(allnum) {
				destpg = atoi(dest+1);
			}
			else return 0;
		} else {
			return 0;
		}
		gsize bytes_read;
	    gsize bytes_written;
    	GError *error = NULL;			
    	Unicode *utxt = (Unicode*)g_convert((const gchar *)txt, -1, "UCS-4LE","UTF-8", &bytes_read, &bytes_written, &error);
		GooList *children = new GooList();
		for(int i=2;i<n;i++) {
			OutlineItem *child = buildOutlineRec(miniexp_nth(i,exp));
			if(child) children->append(child);
		}
		return new OutlineItem(utxt, (int)bytes_written/4, destpg, children);
	} else {
		//WARNPRINTF("Rong types");
		return 0;
	}
}

Outline* buildOutline(ddjvu_document_t *doc) {
	WARNPRINTF("Building outline");
	miniexp_t r;
	while((r=ddjvu_document_get_outline(doc))==miniexp_dummy) handleDdjvu(TRUE); // handle_ddjvu_message(ctx, TRUE);
	int n=miniexp_length(r);
	GooList *items = new GooList();
	for(int i=1;i<n;i++) {
		OutlineItem *item = buildOutlineRec(miniexp_nth(i,r));
		if(item) items->append(item);
	}
	ddjvu_miniexp_release(doc, r);
	return new Outline(items);
}

PDFDoc::PDFDoc(GooString* file) {
    WARNPRINTF("Opening DjVu document %s", file->getCString());
	pageWidths = pageHeights = pageDpis = pageRotations = 0;
	twoPageMode = endsWith(file->getCString(), "2pg.djvu");
	if(twoPageMode) WARNPRINTF("Opening in two page mode");
	ddjvu_context_t *ctx = globalParams->getContext();
	pdf::Mutex* mtx = globalParams->getMutex();
	mtx->lock();
	doc = ddjvu_document_create_by_filename(ctx, file->getCString(), TRUE);
	if(!doc) {
		ERRORPRINTF("Could not open DjVu document");
		ok = gFalse;
		mtx->unlock();
		return;
	} else {
		ok = gTrue;
	}
	while (!ddjvu_document_decoding_done(doc)) handleDdjvu(TRUE);
  	nPages = ddjvu_document_get_pagenum(doc);
	pageWidths    = new int[nPages];
	pageHeights   = new int[nPages];
	pageDpis      = new int[nPages];
	pageRotations = new int[nPages];
	for(int i=0;i<nPages;i++) pageWidths[i] = -1;
	outline = buildOutline(doc);
	mtx->unlock();
}

PDFDoc::~PDFDoc() {
	WARNPRINTF("Destructing PDFDoc");
	delete[] pageWidths;
	delete[] pageHeights;
	delete[] pageDpis;
	delete[] pageRotations;
	delete outline;
}


RenderRet PDFDoc::displayPage(OutputDev *out, int page, double hDPI, double vDPI,
		int rotate, GBool useMediaBox, GBool crop, GBool printing,
		GBool (*abortCheckCbk)(void *data),
		void* abortCheckCbkData,
		GBool (*annotDisplayDecideCbk) (Annot* annot, void *user_data),
		void *annotDisplayDecideCbkData) {
			pdf::Mutex* mtx = globalParams->getMutex();
			mtx->lock();
			out->renderPage(page, this, hDPI, vDPI, rotate, useMediaBox, crop, printing);			
			mtx->unlock();
			return Render_Done;
		}

void SplashOutputDev::renderPage(int page, PDFDoc *doc, double hDPI, double vDPI,
							int rotate, GBool useMediaBox, GBool crop, GBool printing) {

			WARNPRINTF("SplashOutputDev::renderPage, page: %d, hDPI: %f, vDPI: %f, rotate: %d (stub)", page, hDPI, vDPI, rotate);
			ddjvu_rect_t prect;
			prect.x = 0;
			prect.y = 0;
			prect.w = (int)(doc->getPageCropWidth(page) * hDPI / 72.0);
			prect.h = (int)(doc->getPageCropHeight(page) * vDPI / 72.0);								
			bool leftPage = page % 2 != 0;
			if(doc->isTwoPageMode()) page = (page+1)/2;
			ddjvu_page_t *pg = ddjvu_page_create_by_pageno(doc->getDoc(), page-1);
			while (!ddjvu_page_decoding_done(pg)) handleDdjvu(TRUE);

			// restrict max size for wonky dpi settings
			if(prect.w > 2000) { prect.h = (prect.h * 2000) / prect.w; prect.w = 2000; }
			if(prect.h > 2500) { prect.w = (prect.w * 2500) / prect.h; prect.h = 2500; }
			SplashBitmap *bmp = new SplashBitmap(prect.w, prect.h);
			unsigned char *data = bmp->getDataPtr();
			ddjvu_format_style_t style = DDJVU_FORMAT_GREY8;
			ddjvu_render_mode_t mode = DDJVU_RENDER_COLOR;
			ddjvu_format_t *fmt;
			fmt = ddjvu_format_create(style, 0, 0);
			ddjvu_format_set_row_order(fmt, 1);
			int rowsize = prect.w;
			ddjvu_rect_t rrect;
			rrect.y = 0;
			rrect.h = prect.h;
			if(doc->isTwoPageMode()) {
				rrect.w = prect.w;
				prect.w *= 2;
				rrect.x = leftPage ? 0 : prect.w - rrect.w;
			} else {
				rrect.x = 0;
				rrect.w = prect.w;
			}
			if(!ddjvu_page_render(pg, mode, &prect, &rrect, fmt, rowsize, (char*)data)) {
				unsigned char white = 0xFF;
				memset(data, white, rowsize * prect.h);
				WARNPRINTF("PDFDoc::displayPage: error displaying DjVu page %d, showing white bitmap", page);
				// ddjvu_context_t *ctx = globalParams->getContext();
				// ddjvu_message_t *msg = ddjvu_message_peek(ctx);
				// if(msg && msg->m_any.tag == DDJVU_ERROR) WARNPRINTF("ddjvu: %s\n", msg->m_error.message);
			}
			ddjvu_format_release(fmt);
			ddjvu_page_release(pg);
			// bitmap improvement?
			// experimental!
			// int w = (int)rrect.w;
			// for(int i=w*(rrect.h-1)-2;i>=w;i--) {
				// data[i] = (unsigned char)(256.0 * pow((data[i]/256.0),1.7));
				//if(data[i-1] > data[i] && data[i+1] > data[i]) data[i] >>= 1;
				// if(data[i-1] > data[i] && data[i+1] > data[i] && data[i+w] < 192 && data[i-w] < 192) data[i] >>= 1;
				//if(data[i] > 128) data[i] = 192 + (data[i]-128)/2;
				//else data[i] = data[i]/2;
			// }
			// end of bitmap improvement
			setBitmap(bmp);
		}



void addWords(miniexp_t exp, std::vector<TextWord*> &words, double svDPI, double shDPI, int pageWidth, int pageHeight, int realRotate) {
	if(!miniexp_consp(exp)) {
		// WARNPRINTF("Not a list or empty list!");
		return;
	}
	// WARNPRINTF("addWord");
	const char* expName = miniexp_to_name(miniexp_nth(0,exp));
	if(!expName) {
		WARNPRINTF("Not a symbol: %s", miniexp_to_str(exp));
		return;
	}
	if(strcmp(expName, "word") == 0) {
		// WARNPRINTF("adding word");
		int n = miniexp_length(exp);
		if(n == 6) {
			if(miniexp_numberp(miniexp_nth(1,exp)) &&
			   miniexp_numberp(miniexp_nth(2,exp)) &&
			   miniexp_numberp(miniexp_nth(3,exp)) &&
			   miniexp_numberp(miniexp_nth(4,exp)) &&
			   miniexp_stringp(miniexp_nth(5,exp))) {
				int x1 = miniexp_to_int(miniexp_nth(1,exp));
				int y1 = miniexp_to_int(miniexp_nth(2,exp));
				int x2 = miniexp_to_int(miniexp_nth(3,exp));
				int y2 = miniexp_to_int(miniexp_nth(4,exp));
				const char* word = miniexp_to_str(miniexp_nth(5,exp));
				// WARNPRINTF("Found word: %s", word);
				//double d = pDPI;
				// PDFRectangle rect = PDFRectangle(72*x1/d,72*y1/d,72*x2/d,72*y2/d);  // fixme scale location?
				// fixme incorrect calculation!
				if(realRotate == 1) { // fixme move to separate function
					int tmp1=y1;
					int tmp2=y2;
					y1=x1;//pageHeight-x1;
					y2=x2; //pageHeight-x2;
					x1=pageWidth-tmp2;
					x2=pageWidth-tmp1;
				} else if(realRotate == 3) {
					int tmp1=y1;
					int tmp2=y2;
					y1=pageHeight-x2;
					y2=pageHeight-x1;
					x1=tmp1;
					x2=tmp2;
				} else if(realRotate == 2) {
					int tmp1=x1;
					x1=pageWidth-x2;
					x2=pageWidth-tmp1;
					tmp1=y1;
					y1=pageHeight-y2;
					y2=pageHeight-tmp1;
				}
				PDFRectangle rect = PDFRectangle(shDPI*x1-1,svDPI*(pageHeight-y2)-1,shDPI*x2+1,svDPI*(pageHeight-y1)+1);  // fixme scale location?
				words.push_back(new TextWord(word, &rect));
			   } else {
				   WARNPRINTF("Wrong type for s-expression");
			   }
		} else {
			WARNPRINTF("Wrong list size for word annotation!");
		}
	} else {
		// WARNPRINTF("non-word: %s, recursing", expName);
		int n = miniexp_length(exp);
		miniexp_t sub;
		for(int i=1;i<n;i++) {
		 	sub = miniexp_nth(i,exp);
			addWords(sub, words, svDPI, shDPI, pageWidth, pageHeight, realRotate);
		}
	}
}

TextWordList *makeWordList(miniexp_t exp, double svDPI, double shDPI, int pageWidth, int pageHeight, int realRotate) {
	std::vector<TextWord*> words;
	addWords(exp, words, svDPI, shDPI, pageWidth, pageHeight, realRotate);
	TextWord** ws = new TextWord*[words.size()];
	for(int i=0;i<(int)words.size();i++) {
		ws[i] = words[i];
	}
	return new TextWordList(ws, words.size());
}

void TextOutputDev::renderPage(int page, PDFDoc *doc, double hDPI, double vDPI,
							int rotate, GBool useMediaBox, GBool crop, GBool printing) {
	WARNPRINTF("TextOutputDev::renderPage, page: %d, hDPI: %f, vDPI: %f, rotate: %d (stub)", page, hDPI, vDPI, rotate);

	double shDPI = hDPI / doc->getPageDPI(page);
	double svDPI = vDPI / doc->getPageDPI(page);
	double pageMiddle = (shDPI * doc->getPageWidthPixels(page))/2.0;
	int pageWidth = doc->getPageWidthPixels(page);
	int pageHeight = doc->getPageHeightPixels(page);
	int realRotate = doc->getPageRealRotate(page);
	// WARNPRINTF("Page: %d, width: %d, height: %d real rotation: %d", page, realRotate);
	bool isLeftPage = page % 2 != 0;
	if(doc->isTwoPageMode()) page = (page+1)/2;
	ddjvu_document_t *ddoc = doc->getDoc();
	miniexp_t r;
	while((r=ddjvu_document_get_pagetext(ddoc,page-1,0))==miniexp_dummy) handleDdjvu(TRUE); // handle_ddjvu_message(ctx, TRUE);
	//WARNPRINTF("Making word list for %f dpi", shDPI);
	TextWordList *wl = makeWordList(r,shDPI,svDPI,pageWidth,pageHeight,realRotate);
	ddjvu_miniexp_release(ddoc, r);
	if(doc->isTwoPageMode()) {
		std::vector<TextWord*> foundWords;
		for(int i=0;i<wl->getLength();i++) {
			TextWord* w = wl->get(i);
			double x1,y1,x2,y2;
			w->getBBox(&x1,&y1,&x2,&y2);
			if(isLeftPage) {
				if(x2 < pageMiddle) {
					foundWords.push_back(new TextWord(*w));
				}
			} else {			
				if(x1 > pageMiddle) {
					PDFRectangle rect(x1-pageMiddle,y1,x2-pageMiddle,y2);
					foundWords.push_back(new TextWord(w->getText()->getCString(),&rect));
				}
			}
		}
		delete wl;
		TextWord **ws = new TextWord*[foundWords.size()];
		for(int i=0;i<(int)foundWords.size();i++) ws[i] = foundWords[i];
		wl = new TextWordList(ws, foundWords.size());
	}
	setText (new TextPage(wl));								
	//WARNPRINTF("RenderPage done");
}
								
SplashBitmap::SplashBitmap(int wa, int ha) {
	// WARNPRINTF("creating SplashBitmap: w=%d h=%d", wa,ha);
	data = new unsigned char[wa*ha];
	w = wa;
	h = ha;
	rowSize = wa;
}

SplashBitmap *SplashOutputDev::takeBitmap() {
	WARNPRINTF("SplashOutputDev::takeBitmap");
	if(bmp) {
		SplashBitmap *tmp = bmp;
		bmp = 0;
		return tmp;
	}
	return 0;
}

TextPage *TextOutputDev::takeText() {
	WARNPRINTF("TextOutputDev::takeText");
	if(text) {
		TextPage *tmp = text;
		text = 0;
		return tmp;
	}
	return 0;
}



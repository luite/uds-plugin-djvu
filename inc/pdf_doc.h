/*
 * Actually DjVu-doc, tries to mimic the behaviour of the poppler PDFDoc class
 * and other poppler classes
 */

#ifndef PDF_DOC_H_
#define PDF_DOC_H_

#include <goo/GooString.h>
#include <goo/GooList.h>

#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>

#include "log.h"
#include "mutex.h"

class PDFDoc;

class GlobalParams {
public:
	GlobalParams();
	~GlobalParams();
	ddjvu_context_t* getContext() { return ctx; }
	pdf::Mutex* getMutex() { return mtx; }
private:
	ddjvu_context_t *ctx;
	pdf::Mutex* mtx;
};
extern GlobalParams *globalParams;

class XRef {
	
};

class PDFRectangle {
public:
	double x1,y1,x2,y2;
	PDFRectangle() { x1 = y1 = x2 = y2 = 0.0; }
	PDFRectangle(double x1a, double y1a, double x2a, double y2a) { x1 = x1a; y1 = y1a; x2 = x2a; y2 = y2a; }
	GBool isValid() { return x1 != 0.0 || y1 != 0.0 || x2 != 0.0 || y2 != 0.0; }
};

class TextWord {
public:
	TextWord() { text = new GooString(""); bbox.x1 = 0.0; bbox.y1 = 0.0; bbox.x2 = 0.0; bbox.y2 = 0.0; }
	TextWord(const char *t, PDFRectangle *box) { text= new GooString(t); bbox.x1 = box->x1; bbox.x2 = box->x2; bbox.y1 = box->y1; bbox.y2 = box->y2; }
	TextWord(const TextWord &tw) { 
		//WARNPRINTF("TextWord::copyconstructor");
		text = tw.text->copy();
		copyBbox(tw);
	}
	TextWord &operator=(const TextWord &tw) {
		delete text;
		text = tw.text->copy();
		copyBbox(tw); 
		// WARNPRINTF("TextWord::operator=, %s", text->getCString());		
		return *this;
	}
	~TextWord() { delete text; }
	int getLength() { return text->getLength(); }
	GooString *getText() { return new GooString(text); }
	void getBBox(double* xMinA, double* yMinA, double* xMaxA, double* yMaxA) { 
		// WARNPRINTF("TextWord::getBBox (%s) (%f,%f)-(%f,%f)",text->getCString(),bbox.x1,bbox.y1,bbox.x2,bbox.y2);
		*xMinA = bbox.x1; *yMinA=bbox.y1; *xMaxA = bbox.x2; *yMaxA = bbox.y2;
	}
	void getCharBBox(int charIdx, double* xMinA, double* yMinA, double* xMaxA, double* yMaxA) {
		// DjVu does not store position per character, approximate:
		int l = text->getLength();
		double w  = (bbox.x2 - bbox.x1) / (double)l;
		double x1 = bbox.x1 + (double)charIdx * w;
		*xMinA = x1; 
		*yMinA = bbox.y1;
		*xMaxA = x1+w;
		*yMaxA = bbox.y2;
	}
private:
	void copyBbox(const TextWord &tw) { bbox.x1 = tw.bbox.x1; bbox.x2 = tw.bbox.x2; bbox.y1 = tw.bbox.y1; bbox.y2 = tw.bbox.y2; }
	GooString *text;
	PDFRectangle bbox;
};

class TextWordList {
public:
	TextWordList(TextWord** w, int l) { words = w; length = l; }
	TextWordList(const TextWordList &twl) {
		/*WARNPRINTF("TextWordList::copyconstructor, length: %d", twl.getLength());
		for(int i=0;i<twl.getLength();i++) {
			GooString* w = twl.get(i)->getText();
			WARNPRINTF("Copy word: %s", w->getCString());
			delete w;
		}*/
		words = new TextWord*[twl.getLength()];
		for(int i=0;i<twl.getLength();i++) words[i] = new TextWord(*twl.words[i]);
		length = twl.getLength();
	}
	TextWordList &operator=(const TextWordList &twl) {
		WARNPRINTF("TextWordList::operator=");
		for(int i=0;i<length;i++) delete words[i]; 
		delete[] words;
		words = new TextWord*[twl.getLength()];
		for(int i=0;i<twl.getLength();i++) words[i] = new TextWord(*twl.words[i]);
		length = twl.getLength();
		return *this;
	}
	~TextWordList() { for(int i=0;i<length;i++) delete words[i]; 
					  delete[] words; }
	int getLength() const { /* WARNPRINTF("TextWordList::length: %d", length); */ return length; }
	TextWord* get(int idx) const { return words[idx]; } // is this correct?
private:
	TextWord **words;
	int length;	
};


class Ref {
public:
	int num;
	int gen;
};



typedef unsigned int Unicode;

enum LinkActionKind { actionGoTo };

class LinkAction {
public:
	LinkActionKind getKind() { return actionGoTo; }
};

class LinkDest {
public:
	LinkDest(int page) {
		ref.gen = 0;
		ref.num = page;
	}
	GBool isPageRef() { /* WARNPRINTF("LinkDest::isPageRef"); */ return gTrue; }
	Ref getPageRef() { /* WARNPRINTF("LinkDest::getPageRef (ref.num=%d, ref.gen=%d)", ref.num, ref.gen); */ return ref; }
private:
	Ref ref;
};

class LinkGoTo : public LinkAction {
public:
	LinkGoTo(int page) {
		dest = new LinkDest(page);
		name = new GooString("#");
		// name->append("10"); // fixme doesn't seem to format?
		// name->appendf("%d", page);
	}
	~LinkGoTo() { delete dest; delete name; }
	LinkDest* getDest() { /* WARNPRINTF("LinkGoTo::getDest"); */ return dest; }
	GooString* getNamedDest() { /* WARNPRINTF("LinkGoTo::getNamedDest (%s)", name->getCString()); */ return name; }
private:
	LinkDest* dest;
	GooString* name;
};

class Link {
public:
	void getRect(double *xa1, double *ya1, double *xa2, double *ya2) { WARNPRINTF("Link::getRect"); }
	GBool inRect(double x, double y) { WARNPRINTF("Link::inRect"); return false; }
	LinkAction* getAction() { WARNPRINTF("Link::getAction"); return &action; }
private:
	LinkGoTo action;
};


class Links {
public:
	int getNumLinks() { WARNPRINTF("Links::getNumLinks"); return 0; }
	Link* getLink(int i) const { WARNPRINTF("Links::getLink"); return 0; }
	GBool onLink(double x, double y) const { WARNPRINTF("Links::onLink"); return false; }
};


class OutlineItem {
public:
	OutlineItem(Unicode* txt, int l, int page, GooList *k) { 
		//WARNPRINTF("Constructing OutlineItem"); 
		unic = txt;
		length = l;
		kids = k;
		action = new LinkGoTo(page);
	}
	~OutlineItem() { /* WARNPRINTF("Destructing OutlineItem"); */
		delete[] unic;
		delete action;
		deleteGooList(kids,OutlineItem);
	}
	LinkAction* getAction() { return action; }
	Unicode* getTitle() { return unic; }
	int getTitleLength() { return length; }
	void open() { /*WARNPRINTF("OutlineItem::open"); */ }
	void close() { /* WARNPRINTF("OutlineItem::close"); */ }
	GooList *getKids() { return kids; }
private:
	int length;
	Unicode *unic;
	LinkAction *action;
	GooList *kids;
};


class Outline {
public:
	Outline(GooList* aItems) {
		items = aItems;
		kids = 0;
	}
	~Outline() { deleteGooList(items, OutlineItem); }
	GooList *getItems() { return items; }
	void open() { }
	void close() { }
	GooList *getKids() { /* return kids; */ return 0; }
private:
	GooList *items;
	GooList *kids; // 0 if no kids?
};



enum SplashColorMode { splashModeMono8 };
typedef Guchar *SplashColorPtr;

// only gray8 color model supported
class SplashBitmap {
public:
	SplashBitmap(int w, int h);
	~SplashBitmap() { delete[] data; }
	int getWidth() { return w; }
	int getHeight() { return h; }
	void getPixel(int x, int y, SplashColorPtr pxl) { pxl[0] = data[x+w*y]; }
	int getRowSize() { return rowSize; }
	SplashColorPtr getDataPtr() { return data; }
private:
	int w;
	int h;
	int rowSize;
	SplashColorPtr data;
};

class OutputDev {
public:
	OutputDev() { WARNPRINTF("Creating OutputDev");  }
	virtual ~OutputDev() { WARNPRINTF("Destructing OutputDev");  }
	void startDoc(XRef *xref) { WARNPRINTF("OutputDev::startDoc"); } // unused, so no need for virtual
	virtual void renderPage(int page, PDFDoc *doc, double hDPI, double vDPI, int rotate, GBool useMediaBox, GBool crop, GBool printing) = 0;
private:

};

class SplashOutputDev : public OutputDev {
public:
	SplashOutputDev(SplashColorMode colorModeA, int bitmapRowPadA,
					GBool reverseVideoA, SplashColorPtr paperColorA) : OutputDev() {
						bmp = 0;
						defCtm[0] = 1.0;
						defCtm[1] = 0.0;
						defCtm[2] = 0.0;
						defCtm[3] = 1.0;
						defCtm[4] = 0.0;
						defCtm[5] = 0.0;
						defIctm[0] = 1.0;
						defIctm[1] = 0.0;
						defIctm[2] = 0.0;
						defIctm[3] = 1.0;
						defIctm[4] = 0.0;
						defIctm[5] = 0.0;	
					}
	virtual ~SplashOutputDev() { if(bmp) delete bmp; }
	double* getDefCTM() { return defCtm; }
	double* getDefICTM() { return defIctm; }
	SplashBitmap* takeBitmap();
	virtual void renderPage(int page, PDFDoc *doc, double hDPI, double vDPI, int rotate, GBool useMediaBox, GBool crop, GBool printing);
private:
	void setBitmap(SplashBitmap *b) { if(bmp) delete bmp; bmp = b; }
	double defCtm[6];  // coordinate transform matrix
	double defIctm[6]; // inverse coordinate transform matrix
	//void setBitmap(SplashBitmap *b) { if(bmp) delete bmp; bmp = b; }
	SplashBitmap *bmp;

};

class TextPage {
public:
	TextPage(TextWordList* w) { wl = w; }
	~TextPage() { delete wl; }
	TextWordList *makeWordList(GBool physLayout) { return new TextWordList(*wl); } // fixme should this create a copy?
private:
	TextWordList *wl;
};

class TextOutputDev : public OutputDev {
public:
	TextOutputDev(char *fileName, GBool physLayoutA,
				  GBool rawOrderA, GBool append) : OutputDev() { text = 0; }
	virtual ~TextOutputDev() { if(text) delete text; }
	TextPage* takeText();
	virtual void renderPage(int page, PDFDoc *doc, double hDPI, double vDPI, int rotate, GBool useMediaBox, GBool crop, GBool printing);
private:
	void setText(TextPage *t) { if(text) delete text; text = t; }
	TextPage* text;	
};


enum RenderRet {
	Render_Done = 0,
	Render_Error = 1,
	Render_Abort = 2,
	Render_Invalid
};


class Annot {
	
};

class PDFDoc {
public:	
	PDFDoc(GooString *fileNameA);
	~PDFDoc();
	GBool isOk() { WARNPRINTF("PDFDoc::isOk"); return ok; }
	int getNumPages() { 
		if(twoPageMode) return 2*nPages;
		return nPages;
	}
	double getPageCropWidth(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
		/* if(pageRotations[page-1] != 0) {
			return 72.0 * (double)pageHeights[page-1] / (double)pageDpis[page-1]; // fixme
		}*/
		if(twoPageMode) return 0.5 * 72.0 * (double)pageWidths[page-1] / (double)pageDpis[page-1];
		return 72.0 * (double)pageWidths[page-1] / (double)pageDpis[page-1];
	}
	double getPageCropHeight(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
		/*if(pageRotations[page-1] != 0) {
			return 72.0 * (double)pageWidths[page-1] / (double)pageDpis[page-1];
		}*/
		return 72.0 * (double)pageHeights[page-1] / (double)pageDpis[page-1];
	}
	double getPageDPI(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
		return pageDpis[page-1];
	}
	int getPageHeightPixels(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
		return pageHeights[page-1];
	}
	int getPageWidthPixels(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
		return pageWidths[page-1];  // always whole image!
	}
	int getPageRotate(int page) {
		return 0;                   // present unrotated pages to uds
	}
	int getPageRealRotate(int page) {
		if(twoPageMode) page = (page+1) / 2;
		refreshPage(page-1);
 // already rotated? (should still use this for annotation coordinates!)
		return pageRotations[page-1];
	} // fixme check value?
	Links* getLinks(int page) { WARNPRINTF("PDFDoc::getLinks(%d)", page); return 0; } // fixme
	Links* generateLinks(int page) { WARNPRINTF("PDFDoc::generateLinks(%d)", page); return 0; } // added by iRex, generate links without rendering?
	int findPage(int num, int gen) { 
		// WARNPRINTF("PDFDoc::findPage(%d, %d)",num,gen); 
		if(twoPageMode) return 2*num-1; // links in toc are to original DjVu page numbers
		return num; 
	} // find page given object id, 0 = not found
	LinkDest* findDest(GooString *name) { WARNPRINTF("PDFDoc::findDest(\"%s\")",name->getCString()); return 0; } // not a destination	
	Outline* getOutline() { return outline; }
	RenderRet displayPage(OutputDev *out, int page, double hDPI, double vDPI,
				int rotate, GBool useMediaBox, GBool crop, GBool printing,
				GBool (*abortCheckCbk)(void *data) = 0,
				void* abortCheckCbkData = 0,
				GBool (*annotDisplayDecideCbk) (Annot* annot, void *user_data) = 0,
				void *annotDisplayDecideCbkData = 0);
			
	XRef *getXRef() { return &xref; }  // get xref table
	bool isTwoPageMode() { return twoPageMode; }
	ddjvu_document_t *getDoc() { return doc; }

private:
	// should this be mutexed?
	void refreshPage(int page) { 
		if(pageWidths[page] == -1) {
			ddjvu_pageinfo_t pi;
			ddjvu_document_get_pageinfo(doc,page,&pi);
			pageWidths[page] = pi.width;
			pageHeights[page] = pi.height;
			pageDpis[page] = pi.dpi;
			pageRotations[page] = pi.rotation;
			// WARNPRINTF("pageinfo: p: %d, w: %d, h: %d, dpi: %d", page, pi.width, pi.height, pi.dpi);
		}
	}
		
	GBool twoPageMode; // artificially split pages
	ddjvu_document_t *doc;
	int nPages;
	GBool ok;
	XRef xref;
	Outline* outline;

	int *pageWidths; // -1 if info for this page not loaded
	int *pageHeights;
	int *pageDpis;
	int *pageRotations;
};

#define splashMaxColorComps 4
typedef Guchar SplashColor[splashMaxColorComps];
typedef Guchar *SplashColorPtr;



#endif

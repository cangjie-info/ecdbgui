#ifndef GRAPH_TRANSCRIPTION_H 
#define GRAPH_TRANSCRIPTION_H

#include <QString>

//class representing  ec.inscriptionGraphs.markup and .graphemeId,
//i.e. the transcription of one graph in an inscription

//TODO - add punc and lingval
//TODO - add font representation


class GraphTranscription
{
public:
	GraphTranscription(int graphMarkup, int graphemeId); //constructor
        enum markupFlags {
            NO_MARKUP = 0,
            ALL_MARKUP = -1,
            GRAPH_UNCERTAIN = 1,
            CRACK_NUMBER = 2,
            EDS_RESTORATION = 4,
            HEWEN_LEFT = 8,
            HEWEN_RIGHT = 16,
            FORM_UNUSUAL = 32,
            CHONGWEN_LEFT = 64,
            CHONGWEN_RIGHT = 128,
        };
	int getMarkup() const;
	void setMarkup(int newMarkup);
	int getGrapheme() const;
	void setGrapheme(int newGrapheme);
	void report() const;
	void setCanHaveImage(bool can); //set canHanveImage member variable
   void toggleCanHaveImage(); //toggles canHaveImage variable
   bool getCanHaveImage() const; //get canHaveImage member variable
   void setGlyph(QString newGlyph);
   QString getGlyph() const;
private:
   int markup; //from ecdb
   int grapheme; //from ecdb
   QString glyph; //from ecdb, but depends on font setting.
   bool canHaveImage; //used in ecdbgui to mark graphs or other inline elements
		//in the transcription that have no corresponding bounding box
		//in the surface image
		//TODO remove nullBox apparatus from imageLabel.
};
#endif

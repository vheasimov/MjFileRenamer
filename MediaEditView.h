#ifndef MediaEditView_H
#define MediaEditView_H

#include <QTreeView>

class MediaEditView : public QTreeView {	
	Q_OBJECT

public:          
	MediaEditView(QWidget *parent = nullptr);
    ~MediaEditView();
	void writeSettings(); 
	void readSettings();
	QModelIndexList	getSelectedIndexes () const;

};

#endif // MediaEditView_H

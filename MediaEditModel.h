#ifndef MediaEditModel_H
#define MediaEditModel_H

#include <QAbstractItemModel>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileIconProvider>

namespace PREF 
{

	const QString filename = "[N]";
	const QString extension = "[E]";
	const QString counter = "[C]";
	const QString cutLeft = "<<";
	const QString cutRight = ">>";
};


enum class reg : int
{
	NO_REG,
	ALL_DOWN,
	ALL_UP,
	FIRST_UP,
	ALL_FIRST_UP,
	NOT_VALID
};


class MediaEditModel : public QAbstractItemModel
{
    Q_OBJECT

public:
	enum class eHeader {eName, ePriority, eNote, eLastOne};
public: 
	MediaEditModel(QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role);

public: 
	void setData(const QString & folder);
	QString getFileSize(quint64 size) const ;

public slots:
	void refresh();

	void startRenamer(QModelIndexList list, const QString & name, const QString & ext, 
		const QString & repl, const QString & counter, const QString & cut, int regFlag);

	QFileInfo * getPropertyItem(const QModelIndex & index);
private:
	void processing(const QFileInfoList & fileinfo, const QString & name, const QString & ext, 
		const QString & repl, const QString & counter, const QString & cut, int regFlag);

	void regProcessing(QString & name, int regFlag);

private:
	QDir dir_;		
	QFileInfoList list_;
	QFileIconProvider icon_provider_;
};

#endif

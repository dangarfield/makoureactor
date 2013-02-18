#ifndef LGPDIALOG_H
#define LGPDIALOG_H

#include <QtGui>
#include "Lgp.h"

class LgpItemModel : public QAbstractItemModel
{
public:
	LgpItemModel(Lgp *lgp, QObject *parent=0);
	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent=QModelIndex()) const;
	int columnCount(const QModelIndex &parent=QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
private:
	Lgp *lgp;
	QStringList fileList;
	QFileIconProvider iconProvider;
};

class LgpDialog : public QDialog
{
	Q_OBJECT
public:
	LgpDialog(Lgp *lgp, QWidget *parent=0);
private slots:
	void replaceCurrent();
	void pack();
	void setButtonsState();
private:
	Lgp *lgp;
	QTreeView *treeView;
	QPushButton *replaceButton, *packButton;
};

#endif // LGPDIALOG_H
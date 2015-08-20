#ifndef KUNDE_HPP_
#define KUNDE_HPP_

#include <QObject>
#include <qvariant.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>




class Kunde: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int nr READ nr WRITE setNr NOTIFY nrChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(QString ort READ ort WRITE setOrt NOTIFY ortChanged FINAL)


public:
	Kunde(QObject *parent = 0);


	void fillFromMap(const QVariantMap& kundeMap);
	void fillFromForeignMap(const QVariantMap& kundeMap);
	void fillFromCacheMap(const QVariantMap& kundeMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int nr() const;
	void setNr(int nr);
	QString name() const;
	void setName(QString name);
	QString ort() const;
	void setOrt(QString ort);


	// SQL
	static const QString createTableCommand();
	static const QString createParameterizedInsertNameBinding();
	static const QString createParameterizedInsertPosBinding();
	void toSqlCache(QVariantList& nrList, QVariantList& nameList, QVariantList& ortList);
	void fillFromSqlQuery(const QSqlQuery& sqlQuery);
	static void fillSqlQueryPos(const QSqlRecord& record);

	virtual ~Kunde();

	Q_SIGNALS:

	void nrChanged(int nr);
	void nameChanged(QString name);
	void ortChanged(QString ort);
	

private:

	int mNr;
	QString mName;
	QString mOrt;

	Q_DISABLE_COPY (Kunde)
};
Q_DECLARE_METATYPE(Kunde*)

#endif /* KUNDE_HPP_ */


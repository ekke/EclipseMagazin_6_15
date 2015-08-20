#ifndef SCHLAGWORT_HPP_
#define SCHLAGWORT_HPP_

#include <QObject>
#include <qvariant.h>




class Schlagwort: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)


public:
	Schlagwort(QObject *parent = 0);


	void fillFromMap(const QVariantMap& schlagwortMap);
	void fillFromForeignMap(const QVariantMap& schlagwortMap);
	void fillFromCacheMap(const QVariantMap& schlagwortMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	QString text() const;
	void setText(QString text);



	virtual ~Schlagwort();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void textChanged(QString text);
	

private:

	QString mUuid;
	QString mText;

	Q_DISABLE_COPY (Schlagwort)
};
Q_DECLARE_METATYPE(Schlagwort*)

#endif /* SCHLAGWORT_HPP_ */


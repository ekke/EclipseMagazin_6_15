#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <QObject>
#include <qvariant.h>


// forward declaration to avoid circular dependencies
class Auftrag;


class Position: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString bezeichnung READ bezeichnung WRITE setBezeichnung NOTIFY bezeichnungChanged FINAL)
	Q_PROPERTY(double Preis READ Preis WRITE setPreis NOTIFY PreisChanged FINAL)
	Q_PROPERTY(Auftrag* auftragsKopf READ auftragsKopf)


public:
	Position(QObject *parent = 0);


	void fillFromMap(const QVariantMap& positionMap);
	void fillFromForeignMap(const QVariantMap& positionMap);
	void fillFromCacheMap(const QVariantMap& positionMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	QString bezeichnung() const;
	void setBezeichnung(QString bezeichnung);
	double Preis() const;
	void setPreis(double Preis);
	Auftrag* auftragsKopf() const;
	// no SETTER auftragsKopf() is only convenience method to get the parent



	virtual ~Position();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void bezeichnungChanged(QString bezeichnung);
	void PreisChanged(double Preis);
	// no SIGNAL auftragsKopf is only convenience way to get the parent
	

private:

	QString mUuid;
	QString mBezeichnung;
	double mPreis;
	// no MEMBER mAuftragsKopf it's the parent

	Q_DISABLE_COPY (Position)
};
Q_DECLARE_METATYPE(Position*)

#endif /* POSITION_HPP_ */


#ifndef DATAMANAGER_HPP_
#define DATAMANAGER_HPP_

#include <qobject.h>
#include <QtSql/QtSql>

#include "Kunde.hpp"
#include "Auftrag.hpp"
#include "Position.hpp"
#include "Schlagwort.hpp"

class DataManager: public QObject
{
Q_OBJECT

// QDeclarativeListProperty to get easy access from QML
Q_PROPERTY(QDeclarativeListProperty<Kunde> kundePropertyList READ kundePropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Auftrag> auftragPropertyList READ auftragPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Schlagwort> schlagwortPropertyList READ schlagwortPropertyList CONSTANT)

public:
    DataManager(QObject *parent = 0);
    virtual ~DataManager();
    Q_INVOKABLE
    void init();

	
	Q_INVOKABLE
	void fillKundeDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInKundeDataModel(QString objectName, Kunde* listItem);

	Q_INVOKABLE
	void removeItemFromKundeDataModel(QString objectName, Kunde* listItem);

	Q_INVOKABLE
	void insertItemIntoKundeDataModel(QString objectName, Kunde* listItem);

	Q_INVOKABLE
	QList<Kunde*> listOfKundeForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList kundeAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allKunde();

	Q_INVOKABLE
	void deleteKunde();

	// access from QML to list of all Kunde
	QDeclarativeListProperty<Kunde> kundePropertyList();

	Q_INVOKABLE
	Kunde* createKunde();

	Q_INVOKABLE
	void undoCreateKunde(Kunde* kunde);

	Q_INVOKABLE
	void insertKunde(Kunde* kunde);

	Q_INVOKABLE
	void insertKundeFromMap(const QVariantMap& kundeMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteKunde(Kunde* kunde);

	Q_INVOKABLE
	bool deleteKundeByNr(const int& nr);

	Q_INVOKABLE
    Kunde* findKundeByNr(const int& nr);
	
	Q_INVOKABLE
	void fillAuftragDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInAuftragDataModel(QString objectName, Auftrag* listItem);

	Q_INVOKABLE
	void removeItemFromAuftragDataModel(QString objectName, Auftrag* listItem);

	Q_INVOKABLE
	void insertItemIntoAuftragDataModel(QString objectName, Auftrag* listItem);

	Q_INVOKABLE
	void resolveAuftragReferences(Auftrag* auftrag);

	Q_INVOKABLE
	void resolveReferencesForAllAuftrag();

	Q_INVOKABLE
	QList<Auftrag*> listOfAuftragForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList auftragAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allAuftrag();

	Q_INVOKABLE
	void deleteAuftrag();

	// access from QML to list of all Auftrag
	QDeclarativeListProperty<Auftrag> auftragPropertyList();

	Q_INVOKABLE
	Auftrag* createAuftrag();

	Q_INVOKABLE
	void undoCreateAuftrag(Auftrag* auftrag);

	Q_INVOKABLE
	void insertAuftrag(Auftrag* auftrag);

	Q_INVOKABLE
	void insertAuftragFromMap(const QVariantMap& auftragMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteAuftrag(Auftrag* auftrag);

	Q_INVOKABLE
	bool deleteAuftragByNr(const int& nr);

	Q_INVOKABLE
    Auftrag* findAuftragByNr(const int& nr);
	
	Q_INVOKABLE
	void fillSchlagwortDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInSchlagwortDataModel(QString objectName, Schlagwort* listItem);

	Q_INVOKABLE
	void removeItemFromSchlagwortDataModel(QString objectName, Schlagwort* listItem);

	Q_INVOKABLE
	void insertItemIntoSchlagwortDataModel(QString objectName, Schlagwort* listItem);

	Q_INVOKABLE
	QList<Schlagwort*> listOfSchlagwortForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList schlagwortAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allSchlagwort();

	Q_INVOKABLE
	void deleteSchlagwort();

	// access from QML to list of all Schlagwort
	QDeclarativeListProperty<Schlagwort> schlagwortPropertyList();

	Q_INVOKABLE
	Schlagwort* createSchlagwort();

	Q_INVOKABLE
	void undoCreateSchlagwort(Schlagwort* schlagwort);

	Q_INVOKABLE
	void insertSchlagwort(Schlagwort* schlagwort);

	Q_INVOKABLE
	void insertSchlagwortFromMap(const QVariantMap& schlagwortMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteSchlagwort(Schlagwort* schlagwort);

	Q_INVOKABLE
	bool deleteSchlagwortByUuid(const QString& uuid);

	Q_INVOKABLE
	Schlagwort* findSchlagwortByUuid(const QString& uuid);

	Q_INVOKABLE
	void setChunkSize(const int& newChunkSize);

    void initKundeFromCache();
    void initKundeFromSqlCache();
    void initAuftragFromCache();
    void initSchlagwortFromCache();

Q_SIGNALS:

	void addedToAllKunde(Kunde* kunde);
	void deletedFromAllKundeByNr(int nr);
	void deletedFromAllKunde(Kunde* kunde);
	void addedToAllAuftrag(Auftrag* auftrag);
	void deletedFromAllAuftragByNr(int nr);
	void deletedFromAllAuftrag(Auftrag* auftrag);
	void addedToAllSchlagwort(Schlagwort* schlagwort);
	void deletedFromAllSchlagwortByUuid(QString uuid);
	void deletedFromAllSchlagwort(Schlagwort* schlagwort);
    
public slots:
    void onManualExit();

private:

	// DataObject stored in List of QObject*
	// GroupDataModel only supports QObject*
    QList<QObject*> mAllKunde;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Kunde*
    static void appendToKundeProperty(
    	QDeclarativeListProperty<Kunde> *kundeList,
    	Kunde* kunde);
    static int kundePropertyCount(
    	QDeclarativeListProperty<Kunde> *kundeList);
    static Kunde* atKundeProperty(
    	QDeclarativeListProperty<Kunde> *kundeList, int pos);
    static void clearKundeProperty(
    	QDeclarativeListProperty<Kunde> *kundeList);
    QList<QObject*> mAllAuftrag;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Auftrag*
    static void appendToAuftragProperty(
    	QDeclarativeListProperty<Auftrag> *auftragList,
    	Auftrag* auftrag);
    static int auftragPropertyCount(
    	QDeclarativeListProperty<Auftrag> *auftragList);
    static Auftrag* atAuftragProperty(
    	QDeclarativeListProperty<Auftrag> *auftragList, int pos);
    static void clearAuftragProperty(
    	QDeclarativeListProperty<Auftrag> *auftragList);
    QList<QObject*> mAllSchlagwort;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Schlagwort*
    static void appendToSchlagwortProperty(
    	QDeclarativeListProperty<Schlagwort> *schlagwortList,
    	Schlagwort* schlagwort);
    static int schlagwortPropertyCount(
    	QDeclarativeListProperty<Schlagwort> *schlagwortList);
    static Schlagwort* atSchlagwortProperty(
    	QDeclarativeListProperty<Schlagwort> *schlagwortList, int pos);
    static void clearSchlagwortProperty(
    	QDeclarativeListProperty<Schlagwort> *schlagwortList);

    void saveKundeToCache();
    	void saveKundeToSqlCache();
    void saveAuftragToCache();
    void saveSchlagwortToCache();

// S Q L
	QSqlDatabase mDatabase;
    bool mDatabaseAvailable;
    bool initDatabase();
    void bulkImport(const bool& tuneJournalAndSync);
    int mChunkSize;

	QVariantList readFromCache(QString& fileName);
	void writeToCache(QString& fileName, QVariantList& data);
	void finish();
};

#endif /* DATAMANAGER_HPP_ */

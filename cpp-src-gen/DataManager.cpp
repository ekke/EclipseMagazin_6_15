#include <QObject>

#include "DataManager.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/GroupDataModel>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

static QString dbName = "sqlcache.db";

static QString dataAssetsPath(const QString& fileName)
{
    return QDir::currentPath() + "/app/native/assets/datamodel/" + fileName;
}
static QString dataPath(const QString& fileName)
{
    return QDir::currentPath() + "/data/" + fileName;
}
static QString cacheKunde = "cacheKunde.json";
static QString cacheAuftrag = "cacheAuftrag.json";
static QString cacheSchlagwort = "cacheSchlagwort.json";

using namespace bb::cascades;
using namespace bb::data;

DataManager::DataManager(QObject *parent) :
        QObject(parent)
{
    // ApplicationUI is parent of DataManager
    // DataManager is parent of all root DataObjects
    // ROOT DataObjects are parent of contained DataObjects
    // ROOT:
    // Kunde
    // Auftrag
    // Schlagwort

    // register all DataObjects to get access to properties from QML:
	qmlRegisterType<Kunde>("org.ekkescorner.data", 1, 0, "Kunde");
	qmlRegisterType<Auftrag>("org.ekkescorner.data", 1, 0, "Auftrag");
	qmlRegisterType<Position>("org.ekkescorner.data", 1, 0, "Position");
	qmlRegisterType<Schlagwort>("org.ekkescorner.data", 1, 0, "Schlagwort");
	// register all ENUMs to get access from QML
	// useful Types for all APPs dealing with data
	// QTimer
	qmlRegisterType<QTimer>("org.ekkescorner.common", 1, 0, "QTimer");

	// no auto exit: we must persist the cache before
    bb::Application::instance()->setAutoExit(false);
    bool res = QObject::connect(bb::Application::instance(), SIGNAL(manualExit()), this, SLOT(onManualExit()));
    Q_ASSERT(res);


    Q_UNUSED(res);
}

/*
 * loads all data from cache.
 * called from main.qml with delay using QTimer
 * Data with 2PhaseInit Caching Policy will only
 * load priority records needed to resolve from others
 */
void DataManager::init()
{
	// SQL init the sqlite database
	mDatabaseAvailable = initDatabase();
	qDebug() << "SQLite created or opened ? " << mDatabaseAvailable;

    initKundeFromSqlCache();
    initAuftragFromCache();
    initSchlagwortFromCache();
}


//  S Q L
/**
 * OPENs the DATABASE FILE
 *
 * Also initialized the DATABASE CONNECTION (SqlDataAccess),
 * we're reusing for all SQL commands on this database
 */
bool DataManager::initDatabase()
{
    mChunkSize = 10000;
    QString pathname;
    pathname = dataPath(dbName);
    QFile dataFile(pathname);
    if (!dataFile.exists()) {
        // of there's a default at assets we copy it - otherwise a new db will be created later
        QFile assetDataFile(dataAssetsPath(dbName));
        if (assetDataFile.exists()) {
            // copy file from assets to data
            bool copyOk = assetDataFile.copy(pathname);
            if (!copyOk) {
                qDebug() << "cannot copy dataAssetsPath(fileName) to dataPath(fileName)";
            }
        }
    }
    //
    mDatabase = QSqlDatabase::addDatabase("QSQLITE");
    mDatabase.setDatabaseName(dataPath(dbName));
    if (mDatabase.open() == false) {
        const QSqlError error = mDatabase.lastError();
        // you should notify the user !
        qWarning() << "Cannot open " << dbName << ":" << error.text();
        return false;
    }
    qDebug() << "Database opened: " << dbName;
    return true;
}

void DataManager::setChunkSize(const int& newChunkSize)
{
    mChunkSize = newChunkSize;
}

/**
 * tune PRAGMA synchronous and journal_mode for better speed with bulk import
 * see https://www.sqlite.org/pragma.html
 * and http://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite
 *
 * PRAGMA database.synchronous = 0 | OFF | 1 | NORMAL | 2 | FULL;
 * default: FULL
 *
 * PRAGMA database.journal_mode = DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
 * default: DELETE
 */
void DataManager::bulkImport(const bool& tuneJournalAndSync)
{
    QSqlQuery query (mDatabase);
    bool success;
    QString journalMode;
    QString syncMode;
    query.prepare("PRAGMA journal_mode");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA journal_mode";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA journal_mode";
        return;
    }
    journalMode = query.value(0).toString();
    //
    query.clear();
    query.prepare("PRAGMA synchronous");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA synchronous";
        return;
    }
        success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA synchronous";
        return;
    }
    switch (query.value(0).toInt()) {
        case 0:
            syncMode = "OFF";
            break;
        case 1:
            syncMode = "NORMAL";
            break;
        case 2:
            syncMode = "FULL";
            break;
        default:
            syncMode = query.value(0).toString();
            break;
    }
    qDebug() << "PRAGMA current values - " << "journal: " << journalMode << " synchronous: " << syncMode;
    //
    query.clear();
    if (tuneJournalAndSync) {
        query.prepare("PRAGMA journal_mode = MEMORY");
    } else {
        query.prepare("PRAGMA journal_mode = DELETE");
    }
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA set journal_mode";
        return;
    }
    //
    query.prepare("PRAGMA journal_mode");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA journal_mode";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA journal_mode";
        return;
    }
    qDebug() << "PRAGMA NEW VALUE journal_mode: " << query.value(0).toString();
    //
    query.clear();
    if (tuneJournalAndSync) {
        query.prepare("PRAGMA synchronous = OFF");
    } else {
        query.prepare("PRAGMA synchronous = FULL");
    }
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA set synchronous";
        return;
    }
    //
    query.prepare("PRAGMA synchronous");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA synchronous";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA synchronous";
        return;
    }
    switch (query.value(0).toInt()) {
        case 0:
            syncMode = "OFF";
            break;
        case 1:
            syncMode = "NORMAL";
            break;
        case 2:
            syncMode = "FULL";
            break;
        default:
            syncMode = query.value(0).toString();
            break;
    }
    qDebug() << "PRAGMA synchronous NEW VALUE: " << syncMode;
}

void DataManager::finish()
{
    saveKundeToCache();
    saveAuftragToCache();
    // Schlagwort is read-only - not saved to cache
}

/*
 * reads Maps of Kunde in from JSON cache
 * creates List of Kunde*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initKundeFromCache()
{
	qDebug() << "start initKundeFromCache";
    mAllKunde.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheKunde);
    qDebug() << "read Kunde from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Kunde* kunde = new Kunde();
        // Important: DataManager must be parent of all root DTOs
        kunde->setParent(this);
        kunde->fillFromCacheMap(cacheMap);
        mAllKunde.append(kunde);
    }
    qDebug() << "created Kunde* #" << mAllKunde.size();
}

/*
 * queries SELECT * FROM Kunde (SQLite cache)
* creates List of Kunde*  from QSqlQuery
 */
void DataManager::initKundeFromSqlCache()
{
	qDebug() << "start initKunde From S Q L Cache";
	mAllKunde.clear();
    QString sqlQuery = "SELECT * FROM kunde";
    QSqlQuery query (mDatabase);
    query.setForwardOnly(true);
    query.prepare(sqlQuery);
    bool success = query.exec();
    if(!success) {
    	qDebug() << "NO SUCCESS query kunde";
    	return;
    }
    QSqlRecord record = query.record();
    Kunde::fillSqlQueryPos(record);
    while (query.next())
    	{
    		Kunde* kunde = new Kunde();
    		// Important: DataManager must be parent of all root DTOs
    		kunde->setParent(this);
    		kunde->fillFromSqlQuery(query);
    		mAllKunde.append(kunde);
    	}
    qDebug() << "read from SQLite and created Kunde* #" << mAllKunde.size();
}

/*
 * save List of Kunde* to JSON cache
 * convert list of Kunde* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveKundeToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Kunde* #" << mAllKunde.size();
    for (int i = 0; i < mAllKunde.size(); ++i) {
        Kunde* kunde;
        kunde = (Kunde*)mAllKunde.at(i);
        QVariantMap cacheMap;
        cacheMap = kunde->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Kunde* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheKunde, cacheList);
}

/*
 * save List of Kunde* to SQLite cache
 * convert list of Kunde* to QVariantLists for each COLUMN
 * tune PRAGMA journal_mode and synchronous for bulk import into SQLite
 * INSERT chunks of data into SQLite (default: 10k rows at once)
 * 
 * Kunde is read-only Cache - so it's not saved automatically at exit
 */
void DataManager::saveKundeToSqlCache()
{
    qDebug() << "now caching Kunde* #" << mAllKunde.size();
    bulkImport(true);
    bool success = false;
    QSqlQuery query (mDatabase);
    query.prepare("DROP TABLE IF EXISTS kunde");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS DROP kunde";
        bulkImport(false);
        return;
    }
    qDebug() << "table DROPPED kunde";
    // create table
    query.clear();
    query.prepare(Kunde::createTableCommand());
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS CREATE kunde";
        bulkImport(false);
        return;
    }
    qDebug() << "table CREATED kunde";

	qDebug() << "BEGIN INSERT chunks of kunde";
    //
    QVariantList nrList, nameList, ortList;
    QString insertSQL = Kunde::createParameterizedInsertPosBinding();
    int laps = mAllKunde.size()/mChunkSize;
    if(mAllKunde.size()%mChunkSize) {
        laps ++;
    }
    int count = 0;
    qDebug() << "chunks of " << mChunkSize << " laps: " << laps;
    int fromPos = 0;
    int toPos = mChunkSize;
    if(toPos > mAllKunde.size()) {
        toPos = mAllKunde.size();
    }
    while (count < laps) {
    	//
    	query.clear();
    	query.prepare("BEGIN TRANSACTION");
    	success = query.exec();
    	if(!success) {
        	qWarning() << "NO SUCCESS BEGIN TRANSACTION";
        	bulkImport(false);
        	return;
    	}
    	// do it
		nrList.clear();
		nameList.clear();
		ortList.clear();
    	for (int i = fromPos; i < toPos; ++i) {
        	Kunde* kunde;
        	kunde = (Kunde*)mAllKunde.at(i);
        	kunde->toSqlCache(nrList, nameList, ortList);
    	}
        //
    	query.clear();
    	query.prepare(insertSQL);
    	query.addBindValue(nrList);
    	query.addBindValue(nameList);
    	query.addBindValue(ortList);
    	success = query.execBatch();
    	if(!success) {
        	qWarning() << "NO SUCCESS INSERT batch kunde";
        	bulkImport(false);
        	return;
    	}
    	query.clear();
    	query.prepare("END TRANSACTION");
    	success = query.exec();
    	if(!success) {
        	qWarning() << "NO SUCCESS END TRANSACTION";
        	bulkImport(false);
        	return;
    	}
        //
        count ++;
        fromPos += mChunkSize;
        toPos += mChunkSize;
        if(toPos > mAllKunde.size()) {
            toPos = mAllKunde.size();
        }
    }
    qDebug() << "END INSERT chunks of kunde";
    bulkImport(false);
}
/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Kunde*> DataManager::listOfKundeForKeys(
        QStringList keyList)
{
    QList<Kunde*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllKunde.size(); ++i) {
        Kunde* kunde;
        kunde = (Kunde*) mAllKunde.at(i);
        if (keyList.contains(QString::number(kunde->nr()))) {
            listOfData.append(kunde);
            keyList.removeOne(QString::number(kunde->nr()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Kunde: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::kundeAsQVariantList()
{
    QVariantList kundeList;
    for (int i = 0; i < mAllKunde.size(); ++i) {
        kundeList.append(((Kunde*) (mAllKunde.at(i)))->toMap());
    }
    return kundeList;
}

QList<QObject*> DataManager::allKunde()
{
    return mAllKunde;
}

QDeclarativeListProperty<Kunde> DataManager::kundePropertyList()
{
    return QDeclarativeListProperty<Kunde>(this, 0,
            &DataManager::appendToKundeProperty, &DataManager::kundePropertyCount,
            &DataManager::atKundeProperty, &DataManager::clearKundeProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Kunde*
void DataManager::appendToKundeProperty(
        QDeclarativeListProperty<Kunde> *kundeList,
        Kunde* kunde)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(kundeList->object);
    if (dataManagerObject) {
        kunde->setParent(dataManagerObject);
        dataManagerObject->mAllKunde.append(kunde);
        emit dataManagerObject->addedToAllKunde(kunde);
    } else {
        qWarning() << "cannot append Kunde* to mAllKunde "
                << "Object is not of type DataManager*";
    }
}
int DataManager::kundePropertyCount(
        QDeclarativeListProperty<Kunde> *kundeList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(kundeList->object);
    if (dataManager) {
        return dataManager->mAllKunde.size();
    } else {
        qWarning() << "cannot get size mAllKunde " << "Object is not of type DataManager*";
    }
    return 0;
}
Kunde* DataManager::atKundeProperty(
        QDeclarativeListProperty<Kunde> *kundeList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(kundeList->object);
    if (dataManager) {
        if (dataManager->mAllKunde.size() > pos) {
            return (Kunde*) dataManager->mAllKunde.at(pos);
        }
        qWarning() << "cannot get Kunde* at pos " << pos << " size is "
                << dataManager->mAllKunde.size();
    } else {
        qWarning() << "cannot get Kunde* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearKundeProperty(
        QDeclarativeListProperty<Kunde> *kundeList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(kundeList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllKunde.size(); ++i) {
            Kunde* kunde;
            kunde = (Kunde*) dataManager->mAllKunde.at(i);
			emit dataManager->deletedFromAllKundeByNr(kunde->nr());
			emit dataManager->deletedFromAllKunde(kunde);
            kunde->deleteLater();
            kunde = 0;
        }
        dataManager->mAllKunde.clear();
    } else {
        qWarning() << "cannot clear mAllKunde " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Kunde
 * and clears the list
 */
void DataManager::deleteKunde()
{
    for (int i = 0; i < mAllKunde.size(); ++i) {
        Kunde* kunde;
        kunde = (Kunde*) mAllKunde.at(i);
        emit deletedFromAllKundeByNr(kunde->nr());
		emit deletedFromAllKunde(kunde);
        kunde->deleteLater();
        kunde = 0;
     }
     mAllKunde.clear();
}

/**
 * creates a new Kunde
 * parent is DataManager
 * if data is successfully entered you must insertKunde
 * if edit was canceled you must undoCreateKunde to free up memory
 */
Kunde* DataManager::createKunde()
{
    Kunde* kunde;
    kunde = new Kunde();
    kunde->setParent(this);
    kunde->prepareNew();
    return kunde;
}

/**
 * deletes Kunde
 * if createKunde was canceled from UI
 * to delete a previous successfully inserted Kunde
 * use deleteKunde
 */
void DataManager::undoCreateKunde(Kunde* kunde)
{
    if (kunde) {
        qDebug() << "undoCreateKunde " << kunde->nr();
        kunde->deleteLater();
        kunde = 0;
    }
}

void DataManager::insertKunde(Kunde* kunde)
{
    // Important: DataManager must be parent of all root DTOs
    kunde->setParent(this);
    mAllKunde.append(kunde);
    emit addedToAllKunde(kunde);
}

void DataManager::insertKundeFromMap(const QVariantMap& kundeMap,
        const bool& useForeignProperties)
{
    Kunde* kunde = new Kunde();
    kunde->setParent(this);
    if (useForeignProperties) {
        kunde->fillFromForeignMap(kundeMap);
    } else {
        kunde->fillFromMap(kundeMap);
    }
    mAllKunde.append(kunde);
    emit addedToAllKunde(kunde);
}

bool DataManager::deleteKunde(Kunde* kunde)
{
    bool ok = false;
    ok = mAllKunde.removeOne(kunde);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllKundeByNr(kunde->nr());
    emit deletedFromAllKunde(kunde);
    kunde->deleteLater();
    kunde = 0;
    return ok;
}


bool DataManager::deleteKundeByNr(const int& nr)
{
    for (int i = 0; i < mAllKunde.size(); ++i) {
        Kunde* kunde;
        kunde = (Kunde*) mAllKunde.at(i);
        if (kunde->nr() == nr) {
            mAllKunde.removeAt(i);
            emit deletedFromAllKundeByNr(nr);
            emit deletedFromAllKunde(kunde);
            kunde->deleteLater();
            kunde = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillKundeDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllKunde.size(); ++i) {
            	theList.append(mAllKunde.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Kunde for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInKundeDataModel(QString objectName,
        Kunde* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Kunde Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromKundeDataModel(QString objectName, Kunde* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Kunde Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoKundeDataModel(QString objectName, Kunde* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

// nr is DomainKey
Kunde* DataManager::findKundeByNr(const int& nr){
    for (int i = 0; i < mAllKunde.size(); ++i) {
        Kunde* kunde;
        kunde = (Kunde*)mAllKunde.at(i);
        if(kunde->nr() == nr){
            return kunde;
        }
    }
    qDebug() << "no Kunde found for nr " << nr;
    return 0;
}
/*
 * reads Maps of Auftrag in from JSON cache
 * creates List of Auftrag*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initAuftragFromCache()
{
	qDebug() << "start initAuftragFromCache";
    mAllAuftrag.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheAuftrag);
    qDebug() << "read Auftrag from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Auftrag* auftrag = new Auftrag();
        // Important: DataManager must be parent of all root DTOs
        auftrag->setParent(this);
        auftrag->fillFromCacheMap(cacheMap);
        mAllAuftrag.append(auftrag);
    }
    qDebug() << "created Auftrag* #" << mAllAuftrag.size();
}


/*
 * save List of Auftrag* to JSON cache
 * convert list of Auftrag* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveAuftragToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Auftrag* #" << mAllAuftrag.size();
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*)mAllAuftrag.at(i);
        QVariantMap cacheMap;
        cacheMap = auftrag->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Auftrag* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheAuftrag, cacheList);
}


void DataManager::resolveAuftragReferences(Auftrag* auftrag)
{
	if (!auftrag) {
        qDebug() << "cannot resolveAuftragReferences with auftrag NULL";
        return;
    }
    if(auftrag->isAllResolved()) {
	    qDebug() << "nothing to do: all is resolved";
	    return;
	}
    if (auftrag->hasAuftraggeber() && !auftrag->isAuftraggeberResolvedAsDataObject()) {
    	Kunde* auftraggeber;
   		auftraggeber = findKundeByNr(auftrag->auftraggeber());
    	if (auftraggeber) {
    		auftrag->resolveAuftraggeberAsDataObject(auftraggeber);
    	} else {
    		qDebug() << "markAuftraggeberAsInvalid: " << auftrag->auftraggeber();
    		auftrag->markAuftraggeberAsInvalid();
    	}
    }
    if (!auftrag->areTagsKeysResolved()) {
        auftrag->resolveTagsKeys(
                listOfSchlagwortForKeys(auftrag->tagsKeys()));
    }
}
void DataManager::resolveReferencesForAllAuftrag()
{
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*)mAllAuftrag.at(i);
    	resolveAuftragReferences(auftrag);
    }
}
/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Auftrag*> DataManager::listOfAuftragForKeys(
        QStringList keyList)
{
    QList<Auftrag*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*) mAllAuftrag.at(i);
        if (keyList.contains(QString::number(auftrag->nr()))) {
            listOfData.append(auftrag);
            keyList.removeOne(QString::number(auftrag->nr()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Auftrag: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::auftragAsQVariantList()
{
    QVariantList auftragList;
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        auftragList.append(((Auftrag*) (mAllAuftrag.at(i)))->toMap());
    }
    return auftragList;
}

QList<QObject*> DataManager::allAuftrag()
{
    return mAllAuftrag;
}

QDeclarativeListProperty<Auftrag> DataManager::auftragPropertyList()
{
    return QDeclarativeListProperty<Auftrag>(this, 0,
            &DataManager::appendToAuftragProperty, &DataManager::auftragPropertyCount,
            &DataManager::atAuftragProperty, &DataManager::clearAuftragProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Auftrag*
void DataManager::appendToAuftragProperty(
        QDeclarativeListProperty<Auftrag> *auftragList,
        Auftrag* auftrag)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(auftragList->object);
    if (dataManagerObject) {
        auftrag->setParent(dataManagerObject);
        dataManagerObject->mAllAuftrag.append(auftrag);
        emit dataManagerObject->addedToAllAuftrag(auftrag);
    } else {
        qWarning() << "cannot append Auftrag* to mAllAuftrag "
                << "Object is not of type DataManager*";
    }
}
int DataManager::auftragPropertyCount(
        QDeclarativeListProperty<Auftrag> *auftragList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(auftragList->object);
    if (dataManager) {
        return dataManager->mAllAuftrag.size();
    } else {
        qWarning() << "cannot get size mAllAuftrag " << "Object is not of type DataManager*";
    }
    return 0;
}
Auftrag* DataManager::atAuftragProperty(
        QDeclarativeListProperty<Auftrag> *auftragList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(auftragList->object);
    if (dataManager) {
        if (dataManager->mAllAuftrag.size() > pos) {
            return (Auftrag*) dataManager->mAllAuftrag.at(pos);
        }
        qWarning() << "cannot get Auftrag* at pos " << pos << " size is "
                << dataManager->mAllAuftrag.size();
    } else {
        qWarning() << "cannot get Auftrag* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearAuftragProperty(
        QDeclarativeListProperty<Auftrag> *auftragList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(auftragList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllAuftrag.size(); ++i) {
            Auftrag* auftrag;
            auftrag = (Auftrag*) dataManager->mAllAuftrag.at(i);
			emit dataManager->deletedFromAllAuftragByNr(auftrag->nr());
			emit dataManager->deletedFromAllAuftrag(auftrag);
            auftrag->deleteLater();
            auftrag = 0;
        }
        dataManager->mAllAuftrag.clear();
    } else {
        qWarning() << "cannot clear mAllAuftrag " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Auftrag
 * and clears the list
 */
void DataManager::deleteAuftrag()
{
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*) mAllAuftrag.at(i);
        emit deletedFromAllAuftragByNr(auftrag->nr());
		emit deletedFromAllAuftrag(auftrag);
        auftrag->deleteLater();
        auftrag = 0;
     }
     mAllAuftrag.clear();
}

/**
 * creates a new Auftrag
 * parent is DataManager
 * if data is successfully entered you must insertAuftrag
 * if edit was canceled you must undoCreateAuftrag to free up memory
 */
Auftrag* DataManager::createAuftrag()
{
    Auftrag* auftrag;
    auftrag = new Auftrag();
    auftrag->setParent(this);
    auftrag->prepareNew();
    return auftrag;
}

/**
 * deletes Auftrag
 * if createAuftrag was canceled from UI
 * to delete a previous successfully inserted Auftrag
 * use deleteAuftrag
 */
void DataManager::undoCreateAuftrag(Auftrag* auftrag)
{
    if (auftrag) {
        qDebug() << "undoCreateAuftrag " << auftrag->nr();
        auftrag->deleteLater();
        auftrag = 0;
    }
}

void DataManager::insertAuftrag(Auftrag* auftrag)
{
    // Important: DataManager must be parent of all root DTOs
    auftrag->setParent(this);
    mAllAuftrag.append(auftrag);
    emit addedToAllAuftrag(auftrag);
}

void DataManager::insertAuftragFromMap(const QVariantMap& auftragMap,
        const bool& useForeignProperties)
{
    Auftrag* auftrag = new Auftrag();
    auftrag->setParent(this);
    if (useForeignProperties) {
        auftrag->fillFromForeignMap(auftragMap);
    } else {
        auftrag->fillFromMap(auftragMap);
    }
    mAllAuftrag.append(auftrag);
    emit addedToAllAuftrag(auftrag);
}

bool DataManager::deleteAuftrag(Auftrag* auftrag)
{
    bool ok = false;
    ok = mAllAuftrag.removeOne(auftrag);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllAuftragByNr(auftrag->nr());
    emit deletedFromAllAuftrag(auftrag);
    auftrag->deleteLater();
    auftrag = 0;
    return ok;
}


bool DataManager::deleteAuftragByNr(const int& nr)
{
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*) mAllAuftrag.at(i);
        if (auftrag->nr() == nr) {
            mAllAuftrag.removeAt(i);
            emit deletedFromAllAuftragByNr(nr);
            emit deletedFromAllAuftrag(auftrag);
            auftrag->deleteLater();
            auftrag = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillAuftragDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllAuftrag.size(); ++i) {
            	theList.append(mAllAuftrag.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Auftrag for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInAuftragDataModel(QString objectName,
        Auftrag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Auftrag Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromAuftragDataModel(QString objectName, Auftrag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Auftrag Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoAuftragDataModel(QString objectName, Auftrag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

// nr is DomainKey
Auftrag* DataManager::findAuftragByNr(const int& nr){
    for (int i = 0; i < mAllAuftrag.size(); ++i) {
        Auftrag* auftrag;
        auftrag = (Auftrag*)mAllAuftrag.at(i);
        if(auftrag->nr() == nr){
            return auftrag;
        }
    }
    qDebug() << "no Auftrag found for nr " << nr;
    return 0;
}
/*
 * reads Maps of Schlagwort in from JSON cache
 * creates List of Schlagwort*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initSchlagwortFromCache()
{
	qDebug() << "start initSchlagwortFromCache";
    mAllSchlagwort.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheSchlagwort);
    qDebug() << "read Schlagwort from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Schlagwort* schlagwort = new Schlagwort();
        // Important: DataManager must be parent of all root DTOs
        schlagwort->setParent(this);
        schlagwort->fillFromCacheMap(cacheMap);
        mAllSchlagwort.append(schlagwort);
    }
    qDebug() << "created Schlagwort* #" << mAllSchlagwort.size();
}


/*
 * save List of Schlagwort* to JSON cache
 * convert list of Schlagwort* to QVariantList
 * toCacheMap stores all properties without transient values
 * Schlagwort is read-only Cache - so it's not saved automatically at exit
 */
void DataManager::saveSchlagwortToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Schlagwort* #" << mAllSchlagwort.size();
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        Schlagwort* schlagwort;
        schlagwort = (Schlagwort*)mAllSchlagwort.at(i);
        QVariantMap cacheMap;
        cacheMap = schlagwort->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Schlagwort* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheSchlagwort, cacheList);
}

/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Schlagwort*> DataManager::listOfSchlagwortForKeys(
        QStringList keyList)
{
    QList<Schlagwort*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        Schlagwort* schlagwort;
        schlagwort = (Schlagwort*) mAllSchlagwort.at(i);
        if (keyList.contains(schlagwort->uuid())) {
            listOfData.append(schlagwort);
            keyList.removeOne(schlagwort->uuid());
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Schlagwort: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::schlagwortAsQVariantList()
{
    QVariantList schlagwortList;
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        schlagwortList.append(((Schlagwort*) (mAllSchlagwort.at(i)))->toMap());
    }
    return schlagwortList;
}

QList<QObject*> DataManager::allSchlagwort()
{
    return mAllSchlagwort;
}

QDeclarativeListProperty<Schlagwort> DataManager::schlagwortPropertyList()
{
    return QDeclarativeListProperty<Schlagwort>(this, 0,
            &DataManager::appendToSchlagwortProperty, &DataManager::schlagwortPropertyCount,
            &DataManager::atSchlagwortProperty, &DataManager::clearSchlagwortProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Schlagwort*
void DataManager::appendToSchlagwortProperty(
        QDeclarativeListProperty<Schlagwort> *schlagwortList,
        Schlagwort* schlagwort)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(schlagwortList->object);
    if (dataManagerObject) {
        schlagwort->setParent(dataManagerObject);
        dataManagerObject->mAllSchlagwort.append(schlagwort);
        emit dataManagerObject->addedToAllSchlagwort(schlagwort);
    } else {
        qWarning() << "cannot append Schlagwort* to mAllSchlagwort "
                << "Object is not of type DataManager*";
    }
}
int DataManager::schlagwortPropertyCount(
        QDeclarativeListProperty<Schlagwort> *schlagwortList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(schlagwortList->object);
    if (dataManager) {
        return dataManager->mAllSchlagwort.size();
    } else {
        qWarning() << "cannot get size mAllSchlagwort " << "Object is not of type DataManager*";
    }
    return 0;
}
Schlagwort* DataManager::atSchlagwortProperty(
        QDeclarativeListProperty<Schlagwort> *schlagwortList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(schlagwortList->object);
    if (dataManager) {
        if (dataManager->mAllSchlagwort.size() > pos) {
            return (Schlagwort*) dataManager->mAllSchlagwort.at(pos);
        }
        qWarning() << "cannot get Schlagwort* at pos " << pos << " size is "
                << dataManager->mAllSchlagwort.size();
    } else {
        qWarning() << "cannot get Schlagwort* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearSchlagwortProperty(
        QDeclarativeListProperty<Schlagwort> *schlagwortList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(schlagwortList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllSchlagwort.size(); ++i) {
            Schlagwort* schlagwort;
            schlagwort = (Schlagwort*) dataManager->mAllSchlagwort.at(i);
			emit dataManager->deletedFromAllSchlagwortByUuid(schlagwort->uuid());
			emit dataManager->deletedFromAllSchlagwort(schlagwort);
            schlagwort->deleteLater();
            schlagwort = 0;
        }
        dataManager->mAllSchlagwort.clear();
    } else {
        qWarning() << "cannot clear mAllSchlagwort " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Schlagwort
 * and clears the list
 */
void DataManager::deleteSchlagwort()
{
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        Schlagwort* schlagwort;
        schlagwort = (Schlagwort*) mAllSchlagwort.at(i);
        emit deletedFromAllSchlagwortByUuid(schlagwort->uuid());
		emit deletedFromAllSchlagwort(schlagwort);
        schlagwort->deleteLater();
        schlagwort = 0;
     }
     mAllSchlagwort.clear();
}

/**
 * creates a new Schlagwort
 * parent is DataManager
 * if data is successfully entered you must insertSchlagwort
 * if edit was canceled you must undoCreateSchlagwort to free up memory
 */
Schlagwort* DataManager::createSchlagwort()
{
    Schlagwort* schlagwort;
    schlagwort = new Schlagwort();
    schlagwort->setParent(this);
    schlagwort->prepareNew();
    return schlagwort;
}

/**
 * deletes Schlagwort
 * if createSchlagwort was canceled from UI
 * to delete a previous successfully inserted Schlagwort
 * use deleteSchlagwort
 */
void DataManager::undoCreateSchlagwort(Schlagwort* schlagwort)
{
    if (schlagwort) {
        qDebug() << "undoCreateSchlagwort " << schlagwort->uuid();
        schlagwort->deleteLater();
        schlagwort = 0;
    }
}

void DataManager::insertSchlagwort(Schlagwort* schlagwort)
{
    // Important: DataManager must be parent of all root DTOs
    schlagwort->setParent(this);
    mAllSchlagwort.append(schlagwort);
    emit addedToAllSchlagwort(schlagwort);
}

void DataManager::insertSchlagwortFromMap(const QVariantMap& schlagwortMap,
        const bool& useForeignProperties)
{
    Schlagwort* schlagwort = new Schlagwort();
    schlagwort->setParent(this);
    if (useForeignProperties) {
        schlagwort->fillFromForeignMap(schlagwortMap);
    } else {
        schlagwort->fillFromMap(schlagwortMap);
    }
    mAllSchlagwort.append(schlagwort);
    emit addedToAllSchlagwort(schlagwort);
}

bool DataManager::deleteSchlagwort(Schlagwort* schlagwort)
{
    bool ok = false;
    ok = mAllSchlagwort.removeOne(schlagwort);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllSchlagwortByUuid(schlagwort->uuid());
    emit deletedFromAllSchlagwort(schlagwort);
    schlagwort->deleteLater();
    schlagwort = 0;
    return ok;
}

bool DataManager::deleteSchlagwortByUuid(const QString& uuid)
{
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot delete Schlagwort from empty uuid";
        return false;
    }
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        Schlagwort* schlagwort;
        schlagwort = (Schlagwort*) mAllSchlagwort.at(i);
        if (schlagwort->uuid() == uuid) {
            mAllSchlagwort.removeAt(i);
            emit deletedFromAllSchlagwortByUuid(uuid);
            emit deletedFromAllSchlagwort(schlagwort);
            schlagwort->deleteLater();
            schlagwort = 0;
            return true;
        }
    }
    return false;
}


void DataManager::fillSchlagwortDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllSchlagwort.size(); ++i) {
            	theList.append(mAllSchlagwort.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Schlagwort for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInSchlagwortDataModel(QString objectName,
        Schlagwort* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Schlagwort Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromSchlagwortDataModel(QString objectName, Schlagwort* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Schlagwort Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoSchlagwortDataModel(QString objectName, Schlagwort* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}
Schlagwort* DataManager::findSchlagwortByUuid(const QString& uuid){
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot find Schlagwort from empty uuid";
        return 0;
    }
    for (int i = 0; i < mAllSchlagwort.size(); ++i) {
        Schlagwort* schlagwort;
        schlagwort = (Schlagwort*)mAllSchlagwort.at(i);
        if(schlagwort->uuid() == uuid){
            return schlagwort;
        }
    }
    qDebug() << "no Schlagwort found for uuid " << uuid;
    return 0;
}


/*
 * reads data in from stored cache
 * if no cache found tries to get data from assets/datamodel
 */
QVariantList DataManager::readFromCache(QString& fileName)
{
    JsonDataAccess jda;
    QVariantList cacheList;
    QFile dataFile(dataPath(fileName));
    if (!dataFile.exists()) {
        QFile assetDataFile(dataAssetsPath(fileName));
        if (assetDataFile.exists()) {
            // copy file from assets to data
            bool copyOk = assetDataFile.copy(dataPath(fileName));
            if (!copyOk) {
                qDebug() << "cannot copy dataAssetsPath(fileName) to dataPath(fileName)";
                // no cache, no assets - empty list
                return cacheList;
            }
        } else {
            // no cache, no assets - empty list
            return cacheList;
        }
    }
    cacheList = jda.load(dataPath(fileName)).toList();
    return cacheList;
}

void DataManager::writeToCache(QString& fileName, QVariantList& data)
{
    QString filePath;
    filePath = dataPath(fileName);
    JsonDataAccess jda;
    jda.save(data, filePath);
}

void DataManager::onManualExit()
{
    qDebug() << "## DataManager ## MANUAL EXIT";
    finish();
    bb::Application::instance()->exit(0);
}

DataManager::~DataManager()
{
    // clean up
}

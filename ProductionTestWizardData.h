#pragma once
#include <QStringListModel>
#include <QStringList>
#include <QDebug>

class ProductionTestWizardData : public QObject {
    Q_OBJECT
    QStringList list;
    QString subtype = "AA";
    QString serialNumber = "000000000";
    QString productRevision = "1.23456";
    int activeUser = -1;
    bool mdaTestPassed = false;
public:
    ProductionTestWizardData() {
        list << "Arek" << "Adam";
    }

    QStringList usersNamesList() {
        return list;
    }

    Q_INVOKABLE int getActiveUser() const noexcept  {
        return activeUser;
    }

    Q_INVOKABLE QString getSubtype() const noexcept {
        return subtype;
    }

    Q_INVOKABLE QString getSerialNumber() const noexcept  {
        return serialNumber;
    }

    Q_INVOKABLE QString getProductRevision() const noexcept  {
        return productRevision;
    }

    Q_INVOKABLE bool getMdaTestPassed() const noexcept  {
        return mdaTestPassed;
    }

    Q_INVOKABLE void setActiveUser(const int index) noexcept  {
        activeUser = index;
    }


    Q_INVOKABLE void setSubtype(const QString& str) noexcept {
        subtype = str;
    }

    Q_INVOKABLE void setSerialNumber(const QString& str) noexcept  {
        serialNumber = str;
    }

    Q_INVOKABLE void setProductRevision(const QString& str) noexcept  {
        productRevision = str;
    }

    Q_INVOKABLE void setMdaTestPassed(const bool result) noexcept  {
        mdaTestPassed = result;
    }
};

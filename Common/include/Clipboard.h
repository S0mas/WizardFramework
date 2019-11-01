#pragma once
#include <QGuiApplication>
#include <QClipboard>
#include <QObject>

class QmlClipboardAdapter : public QObject {
	Q_OBJECT
public:
	explicit QmlClipboardAdapter(QObject* parent = 0) : QObject(parent) {
		clipboard = QGuiApplication::clipboard();
	}

	Q_INVOKABLE void setText(const QString& text) const {
		clipboard->setText(text, QClipboard::Clipboard);
		clipboard->setText(text, QClipboard::Selection);
	}

	Q_INVOKABLE QString getText() const {
		return clipboard->text();
	}

private:
	QClipboard* clipboard;
};

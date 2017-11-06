#include "DicomPCH.h"
#include "MessageDialog.h"
#include <QtWidgets/QPushButton>

MessageDialog::MessageDialog(QWidget * parent, const QString & title, const QString & content)
	: QDialog(parent)
{
	setupUi(this);
	titleLabel->setText(title);
	textEdit->setText(content);
	textEdit->setAcceptRichText(false);
	setModal(true);

	buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Send"));
}

MessageDialog::~MessageDialog()
{

}

const QString MessageDialog::getText() const
{
	return textEdit->toPlainText();
}
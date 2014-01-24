#include "documentbody.h"
#include <QPainter>
#include "design.h"

//******************************************************************************************************
/*!
 *\class DocumentBody
 *\brief Класс, отображающий "тело" документа.
*/
//******************************************************************************************************

DocumentBody::DocumentBody(QWidget *parent)
    :QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCursor(Qt::ArrowCursor);
}

void DocumentBody::paintEvent(QPaintEvent *)
{

}

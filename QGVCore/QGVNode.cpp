/***************************************************************
QGVCore
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/
#include <QGVNode.h>
#include <QGVCore.h>
#include <QGVScene.h>
#include <QGVGraphPrivate.h>
#include <QGVNodePrivate.h>
#include <QDebug>
#include <QPainter>

QGVNode::QGVNode(QGVNodePrivate *node, QGVScene *scene) : _node(node), _scene(scene)
{
        setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QGVNode::~QGVNode()
{
        _scene->removeItem(this);
        delete _node;
}

QString QGVNode::label() const
{
        return getAttribute("label");
}

void QGVNode::setLabel(const QString &label)
{
        setAttribute("label", label);
}

QRectF QGVNode::boundingRect() const
{
        return _path.boundingRect();
}

void QGVNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        painter->save();

        painter->setPen(_pen);

        if (isSelected()) {
                QBrush tbrush(_brush);
                tbrush.setColor(tbrush.color().darker(120));
                painter->setBrush(tbrush);
        }
        else
                painter->setBrush(_brush);
        if (is_record) {
                QPen tpen(_pen);
                tpen.setColor(Qt::GlobalColor::red);
                painter->setPen(tpen);
                painter->drawRect(_boundingBox);
                painter->setPen(_pen);
                for (auto i : _record_desc) {
                        painter->save();
                        painter->drawRect(i.first);
                        if (!i.second.isEmpty())
                                painter->drawText(i.first, Qt::AlignCenter, i.second);
                        painter->restore();
                }
        } else {
                painter->drawPath(_path);

                painter->setPen(QGVCore::toColor(getAttribute("labelfontcolor")));

                const QRectF rect = boundingRect().adjusted(2, 2, -2, -2); //Margin
                if (_icon.isNull()) {
                        painter->drawText(rect, Qt::AlignCenter, QGVNode::label());
                }
                else {
                        painter->drawText(rect.adjusted(0, 0, 0, -rect.height() * 2 / 3), Qt::AlignCenter,
                                          QGVNode::label());

                        const QRectF img_rect = rect.adjusted(0, rect.height() / 3, 0, 0);
                        QImage       img      = _icon.scaled(img_rect.size().toSize(), Qt::KeepAspectRatio,
                                                             Qt::SmoothTransformation);
                        painter->drawImage(img_rect.topLeft() + QPointF((img_rect.width() - img.rect().width()) / 2, 0),
                                           img);
                }
        }

        painter->restore();
}

void QGVNode::setAttribute(const QString &name, const QString &value)
{
        agsafeset(_node->node(), name.toLocal8Bit().data(), value.toLocal8Bit().data(), "");
}

QString QGVNode::getAttribute(const QString &name) const
{
        char *value = agget(_node->node(), name.toLocal8Bit().data());
        if (value)
                return value;
        return QString();
}

void QGVNode::setIcon(const QImage &icon)
{
        _icon = icon;
}

void QGVNode::updateLayout()
{
        prepareGeometryChange();
        qreal width_orig  = ND_width(_node->node());
        qreal width       = width_orig * (DotDefaultDPI);
        qreal height_orig = ND_height(_node->node());
        qreal height      = height_orig * (DotDefaultDPI);

        //Node Position (center)
        qreal gheight = QGVCore::graphHeight(_scene->_graph->graph());

        qreal   orig_xpos = ND_coord(_node->node()).x;
        qreal   x_pos     = ND_coord(_node->node()).x - width / 2;
        //setX(x_pos);
        qreal   orig_ypos = ND_coord(_node->node()).y;
        qreal   y_pos     = (gheight - ND_coord(_node->node()).y) - height / 2;
        QPointF pos       = QPointF(x_pos, y_pos);
//        QPointF pos = QGVCore::centerToOrigin(QGVCore::toPoint(ND_coord(_node->node()), gheight), width, height);
        setPos(pos);

        qDebug() << "Orig width: " << width_orig << " width: " << width << " Orig Height: " << height_orig <<
        " Height: " << height;
        qDebug() << "Orig X: " << orig_xpos << " X: " << x_pos << " Orig Y: " << orig_ypos << " Y: " << y_pos;


        //Node on top
        setZValue(1);

        //Node path
        if (0 == strcmp(ND_shape(_node->node())->name, "record")) {
                this->is_record = true;
//        _record_desc = QGVCore::to_record_label((field_t *) ND_shape_info(_node->node()), pos.x(), pos.y(), gheight, width, height);
                _record_desc = QGVCore::to_record_label((field_t *) ND_shape_info(_node->node()), gheight, width,
                                                        height);
                QPointF pos = QGVCore::centerToOrigin(QGVCore::toPoint(ND_coord(_node->node()), gheight), width,
                                                      height);
                _boundingBox = QRectF(QPointF(0, 0), QSizeF(width, height));
        } else {
                this->is_record = false;
                _path = QGVCore::toPath(ND_shape(_node->node())->name, (polygon_t *) ND_shape_info(_node->node()),
                                        width, height);
        }
        _pen.setWidth(1);

        _brush.setStyle(QGVCore::toBrushStyle(getAttribute("style")));
        _brush.setColor(QGVCore::toColor(getAttribute("fillcolor")));
        _pen.setColor(QGVCore::toColor(getAttribute("color")));

        setToolTip(getAttribute("tooltip"));
}

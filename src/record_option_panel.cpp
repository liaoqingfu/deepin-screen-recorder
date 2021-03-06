/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2017 Deepin, Inc.
 *               2011 ~ 2017 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include "utils.h"
#include "record_option_panel.h"
#include "constant.h"
#include <DHiDPIHelper>

const int RecordOptionPanel::WIDTH = 124;
const int RecordOptionPanel::HEIGHT = 36;
const int RecordOptionPanel::ICON_OFFSET_X = 14;
    
DWIDGET_USE_NAMESPACE

RecordOptionPanel::RecordOptionPanel(QPushButton *parent) : QPushButton(parent)
{
    installEventFilter(this);
    setMouseTracking(true);

    gifNormalImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("gif_normal.svg"));
    gifPressImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("gif_press.svg"));
    gifCheckedImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("gif_checked.svg"));

    videoNormalImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("video_normal.svg"));
    videoPressImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("video_press.svg"));
    videoCheckedImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("video_checked.svg"));

    settings = new Settings();
    saveAsGif = settings->getOption("save_as_gif").toBool();

    isPressGif = false;
    isPressVideo = false;

    setFixedSize(WIDTH, HEIGHT);
}

RecordOptionPanel::~RecordOptionPanel()
{
    delete settings;
}

void RecordOptionPanel::paintEvent(QPaintEvent *)
{
    // Init.
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw background.
    Utils::drawTooltipBackground(painter, rect());

    // Draw icon.
    qreal devicePixelRatio = qApp->devicePixelRatio();
    painter.setOpacity(1);
    int gifIconX = rect().x() + ICON_OFFSET_X / devicePixelRatio;
    int gifIconY = rect().y() + (rect().height() - gifCheckedImg.height() / devicePixelRatio) / 2;
    int gifTextX = gifIconX + gifCheckedImg.width() / devicePixelRatio;
    int gifTextWidth = rect().width() / 2 - ICON_OFFSET_X / devicePixelRatio - gifCheckedImg.width() / devicePixelRatio;
    QString gifColor;
    if (saveAsGif) {
        painter.drawPixmap(QPoint(gifIconX, gifIconY), gifCheckedImg);
        gifColor = "#217DFF";
    } else if (isPressGif) {
        painter.drawPixmap(QPoint(gifIconX, gifIconY), gifPressImg);
        gifColor = "#004BCA";
    } else {
        painter.drawPixmap(QPoint(gifIconX, gifIconY), gifNormalImg);
        gifColor = "#000000";
    }
    Utils::drawTooltipText(painter, "GIF", gifColor, 9, QRectF(gifTextX, rect().y(), gifTextWidth, rect().height()));

    int videoIconX = rect().x() + rect().width() / 2;
    int videoIconY = rect().y() + (rect().height() - videoCheckedImg.height() / devicePixelRatio) / 2;
    int videoTextX = videoIconX + videoCheckedImg.width() / devicePixelRatio;
    int videoTextWidth = rect().width() / 2 - ICON_OFFSET_X / devicePixelRatio - gifCheckedImg.width() / devicePixelRatio;
    QString videoColor;
    if (!saveAsGif) {
        painter.drawPixmap(QPoint(videoIconX, videoIconY), videoCheckedImg);
        videoColor = "#217DFF";
    } else if (isPressVideo) {
        painter.drawPixmap(QPoint(videoIconX, videoIconY), videoPressImg);
        videoColor = "#004BCA";
    } else {
        painter.drawPixmap(QPoint(videoIconX, videoIconY), videoNormalImg);
        videoColor = "#000000";
    }
    Utils::drawTooltipText(painter, "MP4", videoColor, 9, QRectF(videoTextX, rect().y(), videoTextWidth, rect().height()));
}

bool RecordOptionPanel::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int pressX = mouseEvent->x();

        if (pressX > rect().x() && pressX < rect().x() + rect().width() / 2) {
            isPressGif = true;
            isPressVideo = false;
        } else {
            isPressVideo = true;
            isPressGif = false;
        }
        
        repaint();
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int releaseX = mouseEvent->x();
        
        if (releaseX > rect().x() && releaseX < rect().x() + rect().width() / 2) {
            if (isPressGif) {
                saveAsGif = true;
                settings->setOption("save_as_gif", saveAsGif);
            } else if (isPressVideo) {
                isPressVideo = false;
            }
        } else {
            if (isPressVideo) {
                saveAsGif = false;
                settings->setOption("save_as_gif", saveAsGif);
            } else if (isPressGif) {
                isPressGif = false;
            }
        }
        
        repaint();
    }

    return false;
}

bool RecordOptionPanel::isSaveAsGif()
{
    return saveAsGif;
}    

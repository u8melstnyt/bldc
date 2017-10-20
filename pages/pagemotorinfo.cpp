/*
    Copyright 2016 - 2017 Benjamin Vedder	benjamin@vedder.se
	Copyright 2017 Nico Ackermann	added poles, teeth of pulleys and wheel size to the additional information page and stored it in the settings for the realtime data screen to show speed and distance

    This file is part of VESC Tool.

    VESC Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VESC Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#include "pagemotorinfo.h"
#include "ui_pagemotorinfo.h"
#include "widgets/helpdialog.h"

PageMotorInfo::PageMotorInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageMotorInfo)
{
    ui->setupUi(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    mVesc = 0;
}

PageMotorInfo::~PageMotorInfo()
{
    delete ui;
}

VescInterface *PageMotorInfo::vesc() const
{
    return mVesc;
}

void PageMotorInfo::setVesc(VescInterface *vesc)
{
    mVesc = vesc;

    if (mVesc) {
        ui->descriptionEdit->document()->setHtml(mVesc->mcConfig()->getParamQString("motor_description"));
        ui->qualityEdit->document()->setHtml(mVesc->mcConfig()->getParamQString("motor_quality_description"));

        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_poles");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "wheel_pulley_teeth");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_pulley_teeth");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "wheel_size_in_mm");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_brand");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_model");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_weight");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_sensor_type");
        ui->generalTab->addParamRow(mVesc->mcConfig(), "motor_loss_torque");

        ui->qualityTab->addParamRow(mVesc->mcConfig(), "motor_quality_bearings");
        ui->qualityTab->addParamRow(mVesc->mcConfig(), "motor_quality_magnets");
        ui->qualityTab->addParamRow(mVesc->mcConfig(), "motor_quality_construction");

        if (mSettings.contains("motor_poles")) {
            mVesc->mcConfig()->updateParamInt("motor_poles", mSettings.value("motor_poles", 14).toInt());
        }

        if (mSettings.contains("wheel_pulley_teeth")) {
            mVesc->mcConfig()->updateParamInt("wheel_pulley_teeth", mSettings.value("wheel_pulley_teeth", 36).toInt());
        }

        if (mSettings.contains("motor_pulley_teeth")) {
            mVesc->mcConfig()->updateParamInt("motor_pulley_teeth", mSettings.value("motor_pulley_teeth", 15).toInt());
        }

        if (mSettings.contains("wheel_size_in_mm")) {
            mVesc->mcConfig()->updateParamInt("wheel_size_in_mm", mSettings.value("wheel_size_in_mm", 90).toInt());
        }

        connect(mVesc->mcConfig(), SIGNAL(paramChangedQString(QObject*,QString,QString)),
                this, SLOT(paramChangedQString(QObject*,QString,QString)));
        connect(mVesc->mcConfig(), SIGNAL(savingXml()),
                this, SLOT(savingXml()));

        connect(mVesc->mcConfig(), SIGNAL(paramChangedInt(QObject*,QString,int)),
                this, SLOT(paramChangedInt(QObject*,QString,int)));

    }
}

void PageMotorInfo::savingXml()
{
    mVesc->mcConfig()->updateParamString("motor_description",
                                         ui->descriptionEdit->document()->toHtml(),
                                         this);

    mVesc->mcConfig()->updateParamString("motor_quality_description",
                                         ui->qualityEdit->document()->toHtml(),
                                         this);
}

void PageMotorInfo::paramChangedQString(QObject *src, QString name, QString newParam)
{
    if (src != this) {
        if (name == "motor_description") {
            ui->descriptionEdit->document()->setHtml(newParam);
        } else if (name == "motor_quality_description") {
            ui->qualityEdit->document()->setHtml(newParam);
        }
    }
}

void PageMotorInfo::paramChangedInt(QObject *src, QString name, int newParam)
{
    (void)src;
    (void)newParam;

    if (name == "motor_poles") {
        mSettings.setValue("motor_poles", mVesc->mcConfig()->getParamInt("motor_poles"));
    }

    if (name == "wheel_pulley_teeth") {
        mSettings.setValue("wheel_pulley_teeth", mVesc->mcConfig()->getParamInt("wheel_pulley_teeth"));
    }

    if (name == "motor_pulley_teeth") {
        mSettings.setValue("motor_pulley_teeth", mVesc->mcConfig()->getParamInt("motor_pulley_teeth"));
    }

    if (name == "wheel_size_in_mm") {
        mSettings.setValue("wheel_size_in_mm", mVesc->mcConfig()->getParamInt("wheel_size_in_mm"));
    }
}

void PageMotorInfo::on_descriptionHelpButton_clicked()
{
    if (mVesc) {
        HelpDialog::showHelp(this, mVesc->mcConfig(), "motor_description");
    }
}

/*
    Copyright 2016 - 2017 Benjamin Vedder	benjamin@vedder.se
	Copyright 2017 Nico Ackermann	added additional throttle curve calculation, removed version check

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

#include "util.h"
#include <cmath>
#include <QProgressDialog>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include <QNetworkReply>

namespace util {
double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float throttle_curve(float val, float curve_acc, float curve_brake, int mode) {
    float ret = 0.0;
	
	if (val < -1.0) {
        val = -1.0;
    }

    if (val > 1.0) {
        val = 1.0;
    }
	
    float val_a = fabsf(val);


    float curve;
    if (val >= 0.0) {
        curve = curve_acc;
    } else {
        curve = curve_brake;
    }

    // See
    // http://math.stackexchange.com/questions/297768/how-would-i-create-a-exponential-ramp-function-from-0-0-to-1-1-with-a-single-val
    if (mode == 0) { // Exponential
        if (curve >= 0.0) {
            ret = 1.0 - powf(1.0 - val_a, 1.0 + curve);
        } else {
            ret = powf(val_a, 1.0 - curve);
        }
    } else if (mode == 1) { // Expo-Natural
        float first;
        if (curve >= 0.0) {
            first = 1.0 - powf(1.0 - val_a, 1.0 + curve);
        } else {
            first = powf(val_a, 1.0 - curve);
        }

        float second;
        if (fabsf(curve) < 1e-10) {
            second = val_a;
        } else {
            if (curve >= 0.0) {
                second = 1.0 - ((expf(curve * (1.0 - val_a)) - 1.0) / (expf(curve) - 1.0));
            } else {
                second = (expf(-curve * val_a) - 1.0) / (expf(-curve) - 1.0);
            }
        }

        ret = (first + second) / 2.0;
    } else if (mode == 2) { // Natural
        if (fabsf(curve) < 1e-10) {
            ret = val_a;
        } else {
            if (curve >= 0.0) {
                ret = 1.0 - ((expf(curve * (1.0 - val_a)) - 1.0) / (expf(curve) - 1.0));
            } else {
                ret = (expf(-curve * val_a) - 1.0) / (expf(-curve) - 1.0);
            }
        }
    } else if (mode == 3) { // Polynomial
        if (curve >= 0.0) {
            ret = 1.0 - ((1.0 - val_a) / (1.0 + curve * val_a));
        } else {
            ret = val_a / (1.0 - curve * (1.0 - val_a));
        }
    } else { // Linear
        ret = val_a;
    }

    if (val < 0.0) {
        ret = -ret;
    }

    return ret;
}

bool autoconnectBlockingWithProgress(VescInterface *vesc, QWidget *parent)
{
    if (!vesc) {
        return false;
    }

    QProgressDialog dialog("Autoconnecting...", QString(), 0, 0, parent);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.show();

    bool res = vesc->autoconnect();

    if (!res) {
        vesc->messageDialog(QObject::tr("Autoconnect"),
                            QObject::tr("Could not autoconnect. Make sure that the USB cable is plugged in "
                                        "and that the ESC is powered."),
                            false);
    }

    return res;
}



}

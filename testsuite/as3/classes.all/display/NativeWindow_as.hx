// NativeWindow_as.hx:  ActionScript 3 "NativeWindow" class, for Gnash.
//
// Generated by gen-as3.sh on: 20090514 by "rob". Remove this
// after any hand editing loosing changes.
//
//   Copyright (C) 2009 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

// This test case must be processed by CPP before compiling to include the
//  DejaGnu.hx header file for the testing framework support.

#if flash9
import flash.display.NativeWindow;
import flash.display.MovieClip;
#else
import flash.NativeWindow;
import flash.MovieClip;
#end
import flash.Lib;
import Type;

// import our testing API
import DejaGnu;

// Class must be named with the _as suffix, as that's the same name as the file.
class NativeWindow_as {
    static function main() {
        var x1:NativeWindow = new NativeWindow();

        // Make sure we actually get a valid class        
        if (x1 != null) {
            DejaGnu.pass("NativeWindow class exists");
        } else {
            DejaGnu.fail("NativeWindow lass doesn't exist");
        }
// Tests to see if all the properties exist. All these do is test for
// existance of a property, and don't test the functionality at all. This
// is primarily useful only to test completeness of the API implementation.
	if (x1.active == false) {
	    DejaGnu.pass("NativeWindow::active property exists");
	} else {
	    DejaGnu.fail("NativeWindow::active property doesn't exist");
	}
	if (x1.alwaysInFront == false) {
	    DejaGnu.pass("NativeWindow::alwaysInFront property exists");
	} else {
	    DejaGnu.fail("NativeWindow::alwaysInFront property doesn't exist");
	}
	if (x1.bounds == Rectangle) {
	    DejaGnu.pass("NativeWindow::bounds property exists");
	} else {
	    DejaGnu.fail("NativeWindow::bounds property doesn't exist");
	}
	if (x1.closed == false) {
	    DejaGnu.pass("NativeWindow::closed property exists");
	} else {
	    DejaGnu.fail("NativeWindow::closed property doesn't exist");
	}
	if (x1.displayState == null) {
	    DejaGnu.pass("NativeWindow::displayState property exists");
	} else {
	    DejaGnu.fail("NativeWindow::displayState property doesn't exist");
	}
	if (x1.height == 0) {
	    DejaGnu.pass("NativeWindow::height property exists");
	} else {
	    DejaGnu.fail("NativeWindow::height property doesn't exist");
	}
	if (x1.maximizable == false) {
	    DejaGnu.pass("NativeWindow::maximizable property exists");
	} else {
	    DejaGnu.fail("NativeWindow::maximizable property doesn't exist");
	}
	if (x1.maxSize == Point) {
	    DejaGnu.pass("NativeWindow::maxSize property exists");
	} else {
	    DejaGnu.fail("NativeWindow::maxSize property doesn't exist");
	}
	if (x1.menu == NativeMenu) {
	    DejaGnu.pass("NativeWindow::menu property exists");
	} else {
	    DejaGnu.fail("NativeWindow::menu property doesn't exist");
	}
	if (x1.minimizable == false) {
	    DejaGnu.pass("NativeWindow::minimizable property exists");
	} else {
	    DejaGnu.fail("NativeWindow::minimizable property doesn't exist");
	}
	if (x1.minSize == Point) {
	    DejaGnu.pass("NativeWindow::minSize property exists");
	} else {
	    DejaGnu.fail("NativeWindow::minSize property doesn't exist");
	}
	if (x1.resizable == false) {
	    DejaGnu.pass("NativeWindow::resizable property exists");
	} else {
	    DejaGnu.fail("NativeWindow::resizable property doesn't exist");
	}
	if (x1.stage == Stage) {
	    DejaGnu.pass("NativeWindow::stage property exists");
	} else {
	    DejaGnu.fail("NativeWindow::stage property doesn't exist");
	}
	if (x1.supportsMenu == false) {
	    DejaGnu.pass("NativeWindow::supportsMenu property exists");
	} else {
	    DejaGnu.fail("NativeWindow::supportsMenu property doesn't exist");
	}
	if (x1.supportsNotification == false) {
	    DejaGnu.pass("NativeWindow::supportsNotification property exists");
	} else {
	    DejaGnu.fail("NativeWindow::supportsNotification property doesn't exist");
	}
	if (x1.supportsTransparency == false) {
	    DejaGnu.pass("NativeWindow::supportsTransparency property exists");
	} else {
	    DejaGnu.fail("NativeWindow::supportsTransparency property doesn't exist");
	}
	if (x1.systemChrome == null) {
	    DejaGnu.pass("NativeWindow::systemChrome property exists");
	} else {
	    DejaGnu.fail("NativeWindow::systemChrome property doesn't exist");
	}
	if (x1.systemMaxSize == Point) {
	    DejaGnu.pass("NativeWindow::systemMaxSize property exists");
	} else {
	    DejaGnu.fail("NativeWindow::systemMaxSize property doesn't exist");
	}
	if (x1.systemMinSize == Point) {
	    DejaGnu.pass("NativeWindow::systemMinSize property exists");
	} else {
	    DejaGnu.fail("NativeWindow::systemMinSize property doesn't exist");
	}
	if (x1.title == null) {
	    DejaGnu.pass("NativeWindow::title property exists");
	} else {
	    DejaGnu.fail("NativeWindow::title property doesn't exist");
	}
	if (x1.transparent == false) {
	    DejaGnu.pass("NativeWindow::transparent property exists");
	} else {
	    DejaGnu.fail("NativeWindow::transparent property doesn't exist");
	}
	if (x1.type == null) {
	    DejaGnu.pass("NativeWindow::type property exists");
	} else {
	    DejaGnu.fail("NativeWindow::type property doesn't exist");
	}
	if (x1.visible == false) {
	    DejaGnu.pass("NativeWindow::visible property exists");
	} else {
	    DejaGnu.fail("NativeWindow::visible property doesn't exist");
	}
	if (x1.width == 0) {
	    DejaGnu.pass("NativeWindow::width property exists");
	} else {
	    DejaGnu.fail("NativeWindow::width property doesn't exist");
	}
	if (x1.x == 0) {
	    DejaGnu.pass("NativeWindow::x property exists");
	} else {
	    DejaGnu.fail("NativeWindow::x property doesn't exist");
	}
	if (x1.y == 0) {
	    DejaGnu.pass("NativeWindow::y property exists");
	} else {
	    DejaGnu.fail("NativeWindow::y property doesn't exist");
	}

// Tests to see if all the methods exist. All these do is test for
// existance of a method, and don't test the functionality at all. This
// is primarily useful only to test completeness of the API implementation.
	if (x1.NativeWindow == NativeWindowInitOptions) {
	    DejaGnu.pass("NativeWindow::NativeWindow() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::NativeWindow() method doesn't exist");
	}
	if (x1.activate == null) {
	    DejaGnu.pass("NativeWindow::activate() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::activate() method doesn't exist");
	}
	if (x1.close == null) {
	    DejaGnu.pass("NativeWindow::close() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::close() method doesn't exist");
	}
	if (x1.globalToScreen == Point) {
	    DejaGnu.pass("NativeWindow::globalToScreen() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::globalToScreen() method doesn't exist");
	}
	if (x1.maximize == null) {
	    DejaGnu.pass("NativeWindow::maximize() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::maximize() method doesn't exist");
	}
	if (x1.minimize == null) {
	    DejaGnu.pass("NativeWindow::minimize() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::minimize() method doesn't exist");
	}
	if (x1.notifyUser == null) {
	    DejaGnu.pass("NativeWindow::notifyUser() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::notifyUser() method doesn't exist");
	}
	if (x1.orderInBackOf == false) {
	    DejaGnu.pass("NativeWindow::orderInBackOf() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::orderInBackOf() method doesn't exist");
	}
	if (x1.orderInFrontOf == false) {
	    DejaGnu.pass("NativeWindow::orderInFrontOf() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::orderInFrontOf() method doesn't exist");
	}
	if (x1.orderToBack == false) {
	    DejaGnu.pass("NativeWindow::orderToBack() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::orderToBack() method doesn't exist");
	}
	if (x1.orderToFront == false) {
	    DejaGnu.pass("NativeWindow::orderToFront() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::orderToFront() method doesn't exist");
	}
	if (x1.restore == null) {
	    DejaGnu.pass("NativeWindow::restore() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::restore() method doesn't exist");
	}
	if (x1.startMove == false) {
	    DejaGnu.pass("NativeWindow::startMove() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::startMove() method doesn't exist");
	}
	if (x1.startResize == false) {
	    DejaGnu.pass("NativeWindow::startResize() method exists");
	} else {
	    DejaGnu.fail("NativeWindow::startResize() method doesn't exist");
	}

        // Call this after finishing all tests. It prints out the totals.
        DejaGnu.done();
    }
}

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:

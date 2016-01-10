/** This function defines the Base class, from which all other classes are inherited.
 * The Base class defines a somewhat sound framework for inheritance.
 */
Base = function () {
	if (arguments.length) {
		if (this == window) {
			Base.prototype.extend.call(arguments[0], arguments.callee.prototype);
		} else {
			this.extend(arguments[0]);
		}
	}
};


/** This class property lists the functions to be called when the page
 * containing the code is first loaded.
 */
Base.initialisers = new Array();

/** This class property lists the functions to be called when the page
 * containing the code is unloaded.
 */
Base.destructors = new Array();


/** This class method adds a function to the list of initialisers.
 */
Base.registerInitialiser = function (initialiser) {
	Base.initialisers.push(initialiser);
};

/** This class method adds a function to the list of destructors.
 */
Base.registerDestructor = function (destructor) {
	Base.destructors.push(destructor);
};


Base.prototype = {
	/** The extend() method adds a set of methods and properties to an existing object.
	 * @param	addition	an object containing the methods and properties to add.
	 * @returns			the extended object.
	 */
	extend: function (addition) {
		var _o = Base.prototype.overload;

		var _p = { toSource: null };
		var _l = ['toString', 'valueOf'];
		if (Base._inherits) {
			_l.push('constructor');
		}

		for (var i in _l) {
			var _n = _l[i];
			if (addition[_n] != _p[_n]) {
				_o.call(this, _n, addition[_n]);
				_p[_n] = addition[_n];
			}
		}

		for (var i in addition) {
			if (_p[i]) {
				continue;
			}
			if (addition[i] instanceof Function) {
				_o.call(this, i, addition[i]);
			} else {
				this[i] = addition[i];
			}
		}

		return this;
	},

	/** The overload() method overloads a single method in an object; it
	 * creates a new function that sets this.base to the old value then
	 * calls the new code.
	 * @param	name	the name of the method to be overloaded.
	 * @param	value	the Function object for the new method.
	 * @returns		the value for the overloaded method.
	 */
	overload: function (name, value) {
		if (value == this[name] || name == 'base') {
			return;
		}

		var _v = value, _ov = this[name];
		value = function () {
			var _b = this.base;
			this.base = _ov;
			var _r = _v.apply(this, arguments);
			this.base = _b;
			return _r;
		};
		value.valueOf = function() {
			return _v;
		};
		value.toString = function () {
			return String(_v);
		};

		return this[name] = value;
	}
};


/** This class property stores the list of namespaces to preserve when onunload is called. */
Base._preserve = new Array();


/** This static method is used to define class inheritance.
 * @param	members		an object containing the new methods and properties to be added or overloaded.
 * @param	cMembers	an object containing the methods and properties to be added as static members.
 * @param	name		a string indicating the namespace this object is supposed to define; it is used
 *					to preserve namespaces from being deleted before the onunload() handler.
 * @return			the object corresponding to the newly created class.
 */
Base.inherits = function (members, cMembers, name) {
	if (!members) {
		members = { };
	}

	var _e = Base.prototype.extend;
	var _b = this;

	Base._inherits = true;
	var _p = new _b;
	_e.call(_p, members);
	delete Base._inherits;

	var _c = _p.constructor;
	_p.constructor = this;

	var _cl = function() {
		this.base = _b;
		if (!Base._inherits) {
			_c.apply(this, arguments);
		}
		this.constructor = _cl;
		delete this.base;
	};
	_cl.inherits = this.inherits;
	_cl.toString = function() {
		return String(_c);
	};
	_cl.prototype = _p;

	if (!cMembers) {
		cMembers = { };
	}
	_e.call(_cl, cMembers);

	if (cMembers["onLoad"] instanceof Function) {
		Base.registerInitialiser(_cl["onLoad"]);
	}
	if (cMembers["onUnload"] instanceof Function) {
		Base.registerDestructor(_cl["onUnload"]);
	}

	if (name && name != "") {
		Base._preserve[name] = _cl;
	}

	return _cl;
};


/** Set the page's onload() handler. */
window.onload = function () {
	var _b = Base;
	for (var i in _b.initialisers) {
		if (typeof _b.initialisers[i] == 'undefined') {
			continue;
		}
		_b.initialisers[i]();
	}

	/** Set the page's onunload() handler. */
	window.onunload = function () {
		Base = _b;
		for (var i in _b._preserve) {
			eval(i + " = _b._preserve[i]");
		}

		for (var i = _b.destructors.length - 1; i >= 0; i--) {
			_b.destructors[i]();
		}
	};
};

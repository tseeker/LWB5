LWWidget.Menu = Base.Comp.inherits({

	constructor: function () {
		this.base();

		this.currentMode = null;
		this.modes = new Base.Util.Hashtable();

		this.addSlot('modeDestroyed');
		this.addSlot('draw');
		this.addEvent('Click');
		this.bindEvent('Click', 'draw');
	},

	destroy: function () {
		var modes = this.modes.values();
		for (var i in modes) {
			modes[i].destroy();
		}
		this.base();
	},


	newMode: function (name) {
		if (this.modes.containsKey(name)) {
			return null;
		}

		var mode = new LWWidget.Menu.Mode(name);
		this.modes.put(name, mode);
		if (!this.currentMode) {
			this.currentMode = mode;
		}

		mode.bindEvent('Destroy', 'modeDestroyed', this);
		mode.bindEvent('Click', 'onClick', this);

		return mode;
	},

	setMode: function (name) {
		if (!this.modes.containsKey(name) || this.currentMode && this.currentMode.name == name) {
			return;
		}
		this.currentMode = this.modes.get(name);
		this.draw();
	},

	setSelected: function (name) {
		if (this.currentMode) {
			this.currentMode.setSelected(name);
			this.draw();
		}
	},

	modeDestroyed: function (mode) {
		if (!(mode && mode._cid && mode.name && this.modes.containsKey(mode.name))) {
			return;
		}

		this.modes.remove(mode.name);
		if (this.currentMode && this.currentMode._cid == mode._cid) {
			var _v = this.modes.values();
			if (_v.length) {
				this.currentMode = _v[0];
			} else {
				this.currentMode = null;
			}
		}
	},


	draw: function () {
		var e = document.getElementById('mtMenu');
		if (!(e && this.currentMode)) {
			return;
		}
		e.innerHTML = this.currentMode.draw();
	}

});

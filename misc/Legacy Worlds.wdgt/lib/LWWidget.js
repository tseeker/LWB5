LWWidget = Base.inherits({

	constructor: function () {
		this.base();
		LWWidget.main = this;
		if (LWWidget.debug) {
			Base.Log.initLog();
		}

		this.loadPreferences();
		new LWWidget.Main();
	},


	loadPreferences: function () {
		var keys = ['lwLogin', 'lwPassword', 'lwRemAuth', 'lwGame', 'lwRemGame', 'lwColour'];
		var defs = ['', '', '0', '', '0', 'blue'];

		this.preferences = new Base.Util.Hashtable();

		if (window.widget) {
			// If we're really running inside Dashboard, load the preferences
			for (var i in keys) {
				var p = widget.preferenceForKey(keys[i]);
				if (p && p.length > 0) {
					this.preferences.put(keys[i], p);
				} else {
					this.preferences.put(keys[i], defs[i]);
				}
			}
		} else {
			// We're not inside Dashboard. Use defaults.
			for (var i in keys) {
				this.preferences.put(keys[i], defs[i]);
			}
		}
	},


	getPreference: function (key) {
		return (this.preferences.containsKey(key) ? this.preferences.get(key) : null);
	},


	setPreference: function (key, value) {
		if (this.preferences.containsKey(key) && typeof value == 'string') {
			Base.Log.write("Setting preference " + key + " to '" + value + "'");
			this.preferences.put(key, value);
			if (window.widget) {
				widget.setPreferenceForKey(key, value);
			}
		}
	},

	showDebug: function () {
		if (!this.debugPanel) {
			this.debugPanel = new LWWidget.Debug();
		}
		this.debugPanel.show();
	}


}, {

	// Base URL for the LegacyWorlds website.
	base: 'http://www.legacyworlds.com',

	// Enable/disable debugging
	debug: true,

	// This widget's version number
	version: 1,

	// Main widget instance
	main: null,

	// Password storage
	password: '',

	// When the widget page is loaded, initialise the component
	onLoad: function () {
		new LWWidget();
	}

}, "LWWidget");

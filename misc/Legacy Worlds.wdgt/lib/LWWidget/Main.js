LWWidget.Main = Base.Comp.inherits({

	constructor: function () {
		this.base();

		this.addEvent('Show');
		this.addEvent('Hide');
		this.addSlot('draw');
		this.bindEvent('Show', 'draw');

		// Game-related slots
		this.addSlot('login');
		this.addSlot('loadGame');
		this.addSlot('loadData');

		// Version management slots
		this.addSlot('versionError');
		this.addSlot('versionNewAvailable');
		this.addSlot('versionNewRequired');
		this.addSlot('versionOk');

		// Generate the menu
		this.makeMenu();

		// Version checker
		this.versionCheck = new LWWidget.VersionCheck();
		this.versionPage = new LWWidget.NewVersion();
		this.versionCheck.bindEvent('CommError', 'versionError', this);
		this.versionCheck.bindEvent('NewVersion', 'versionNewAvailable', this);
		this.versionCheck.bindEvent('RequiredVersion', 'versionNewRequired', this);
		this.versionCheck.bindEvent('VersionOk', 'versionOk', this);
		this.versionChecked = false;

		// Login page
		this.loginPage = new LWWidget.Login();
		this.loginPage.bindEvent('Login', 'login', this);

		// Game selection page
		this.gameSelector = new LWWidget.GameSelector();
		this.gameSelector.bindEvent('Select', 'loadGame', this);

		// Game display
		this.gameDisplay = new LWWidget.GameDisplay(this);
		this.gameDisplay.bindEvent('Refresh', 'loadData', this);

		// Generic text display
		this.textPage = new LWWidget.TextPage();

		// FIXME: Set Dashboard-specific event handlers
		// this.initDashboard();

		// FIXME: should use saved password if possible
		this.page = this.loginPage;
		this.menu.setMode('out');
		this.state = 0;

		this.onShow();	// FIXME: use Dashboard events if possible
	},


	initDashboard: function () {
		if (window.widget) {
			var _cid = this._cid;
			widget.onshow = function () {
				if (Base) {
					Base.Comp.get(_cid).onShow();
				}
			};
			widget.onhide = function () {
				if (Base) {
					Base.Comp.get(_cid).onHide();
				}
			};
		}
	},


	makeMenu: function () {
		var _m;
		this.menu = new LWWidget.Menu();
		this.addSlot('menuCommand');
		this.menu.bindEvent('Click', 'menuCommand', this);

		// Waiting
		_m = this.menu.newMode('wait');
		_m.addItem(new LWWidget.Menu.Text('Loading ... Please wait ...'));

		// New version available
		_m = this.menu.newMode('nvAvail');
		_m.addItem(new LWWidget.Menu.Text('New version! &nbsp; &nbsp; '));
		_m.addItem(new LWWidget.Menu.Command('continue', 'Continue'));
		_m.addItem(new LWWidget.Menu.Command('logout', 'Log out'));

		// New version required
		_m = this.menu.newMode('nvReq');
		_m.addItem(new LWWidget.Menu.Text('Update required &nbsp; &nbsp; '));
		_m.addItem(new LWWidget.Menu.Command('logout', 'Log out'));

		// Logged out user
		_m = this.menu.newMode('out');
		_m.addItem(new LWWidget.Menu.Text('Please log in'));

		// Game selection
		_m = this.menu.newMode('game');
		_m.addItem(new LWWidget.Menu.Entry('gamesel', 'Game selection'));
		_m.addItem(new LWWidget.Menu.Command('logout', 'Log out'));

		// Error
		_m = this.menu.newMode('error');
		_m.addItem(new LWWidget.Menu.Text('Error! - '));
		_m.addItem(new LWWidget.Menu.Command('logout', 'Log out'));

		// Standard
		_m = this.menu.newMode('std');
		_m.addItem(new LWWidget.Menu.Command('gamesel', 'Game selection'));
		_m.addItem(new LWWidget.Menu.Entry('player', 'Player'));
		_m.addItem(new LWWidget.Menu.Entry('planets', 'Planets'));
		_m.addItem(new LWWidget.Menu.Entry('fleets', 'Fleets'));
		_m.addItem(new LWWidget.Menu.Entry('budget', 'Budget'));
		_m.addItem(new LWWidget.Menu.Entry('tech', 'Research'));
		_m.addItem(new LWWidget.Menu.Entry('msg', 'Messages'));
		_m.addItem(new LWWidget.Menu.Entry('forums', 'Forums'));
		_m.addItem(new LWWidget.Menu.Entry('ticks', 'Ticks'));
		_m.addItem(new LWWidget.Menu.Command('logout', 'Log out'));
	},


	initLoader: function (login, password) {
		this.loader = new LWWidget.Data.Loader(LWWidget.main.getPreference('lwLogin'), LWWidget.main.password);

		var events = ['CommError', 'FatalError', 'Kick', 'Load', 'LoginFailure', 'Maintenance', 'Destroy'];
		var slots = ['commError', 'serverError', 'playerKicked', 'dataLoaded', 'loginFailed', 'serverMaintenance', 'loaderDestroyed'];
		for (var i=0;i<events.length;i++) {
			this.addSlot(slots[i]);
			this.loader.bindEvent(events[i], slots[i], this);
		}
	},


	draw: function () {
		var c = LWWidget.main.getPreference('lwColour');
		var str = '<img src="images/' + c + '.png" />'
			+ '<div id="mainDiv"><table id="mainTable">'
			 + '<tr><td id="mtTitle">Legacy Worlds</td></tr>'
			 + '<tr><td id="mtMenu">&nbsp;</td></tr>'
			 + '<tr><td id="mtContents">&nbsp;</td></tr>'
			+ '</table></div>';
		if (LWWidget.debug) {
			str += '<div id="debugDiv">&nbsp;</div><div id="debugLink"><a href="#" onclick="LWWidget.main.showDebug();return false">Debug mode</a></div>';
		}
		Base.Browser.get().docBody.innerHTML = str;

		this.menu.draw();
		this.page.draw();
	},


	setTextPage: function (mMode, text) {
		this.menu.setMode(mMode);
		this.page = this.textPage;
		this.page.setText(text);
	},

	menuCommand: function (command) {
		if (command == 'logout') {
			if (this.loader) {
				this.loader.destroy();
			}
			if (this.versionCheck) {
				this.versionCheck.stop();
			}
			if (this.state == 3) {
				this.gameDisplay.hide();
			}
			this.state = 0;
			this.versionChecked = false;
			this.loginPage.setError('');
			this.page = this.loginPage;
			this.menu.setMode('out');
			this.page.draw();
		} else if (command == 'continue') {
			this.login();
		} else if (command == 'gamesel') {
			if (this.state == 3) {
				this.page.hide();
			}
			this.state = 1;
			this.setTextPage('wait', 'Connecting to the Legacy Worlds server<br/><br/>Please wait');
			this.loader.load('main/index');
		} else if (this.state == 3) {
			this.gameDisplay.setPage(command);
		}
	},


	versionError: function () {
		// Ignore the error if the version has already been checked
		if (!this.versionChecked) {
			this.commError();
		}
	},

	versionNewAvailable: function (url) {
		// Ignore the new version unless it is the first time we check
		if (this.versionChecked) {
			return;
		}
		this.versionChecked = true;

		this.versionPage.setData(url, false);
		this.page = this.versionPage;
		this.menu.setMode('nvAvail');
		this.page.draw();
	},

	versionNewRequired: function (url) {
		if (this.state == 3) {
			this.gameDisplay.hide();
		}
		this.versionCheck.stop();
		this.versionPage.setData(url, true);
		this.page = this.versionPage;
		this.menu.setMode('nvReq');
		this.page.draw();
	},

	versionOk: function () {
		if (!this.versionChecked) {
			// Continue login
			this.versionChecked = true;
			this.login();
		}
	},


	login: function () {
		this.setTextPage('wait', 'Connecting to the Legacy Worlds server<br/><br/>Please wait');

		// If the version hasn't been checked, well, check it
		if (!this.versionChecked) {
			this.versionCheck.start();
			return;
		}

		// Initialise the loader and get the game list
		this.initLoader();
		this.loader.load('main/index');
	},


	handleGameList: function (data, force) {
		this.state = 1;

		var games = new Array(), auto = null;

		if (!force && LWWidget.main.getPreference('lwRemGame') == '1') {
			auto = LWWidget.main.getPreference('lwGame');
		}

		for (var i in data.contents) {
			// Check if version is supported
			var _v = data.contents[i].getAttribute('version');
			if (!LWWidget.Game.versions().containsKey(_v)) {
				continue;
			}

			var _g = {
				name: data.contents[i].getText(),
				path: data.contents[i].getAttribute('path'),
				version: LWWidget.Game.versions().get(_v)
			};
			if (auto && auto == _g.path) {
				this.loadGame(_g);
				return;
			}
			games.push(_g);
		}

		if (games.length == 0) {
			this.setTextPage('error', 'It appears that you are not registered to any Legacy Worlds game.');
		} else if (!force && games.length == 1) {
			this.loadGame(games[0]);
		} else {
			this.page = this.gameSelector;
			this.menu.setMode('game');
			this.gameSelector.setGames(games);
		}
	},


	loadGame: function (game) {
		this.state = 2;
		this.currentGame = game;
		this.loader.load(game.path + '/play');
	},

	loadData: function (path) {
		if (this.state == 3 && this.loader) {
			this.loader.load(path + '/play');
		}
	},



	commError: function () {
		this.setTextPage('error', '<b>Communications error</b><br/><br/>The server could not be reached or it sent an invalid reply.');
	},

	serverError: function (code, text) {
		var str = '<b>Server error</b><br/><br/>The server encountered an internal error:<br/><i>'
				+ text + '</i><br/>(code: ' + code + ')';
		this.setTextPage('error', str);
	},

	playerKicked: function (reason) {
		var str = (reason == '' ? 'No reason was specified' : ('<b>Reason:</b> ' + reason));
		if (this.state == 3) {
			this.gameDisplay.hide();
		}
		this.setTextPage('error', 'You have been <b>kicked</b> from the game!<br/><br/>' + str);
	},

	loginFailed: function (code) {
		switch (code) {
		case '0':
			if (this.state == 0) {
				this.loader.destroy();
				LWWidget.main.password = '';
				this.loginPage.setError('Invalid username or password');
				this.page = this.loginPage;
				this.page.draw();
			} else {
				this.setTextPage('error', 'Your password has been changed.');
			}
			break;
		case '1':
			this.setTextPage('error', 'The server encountered an internal error while authenticating your account.');
			break;
		case '2':
			this.setTextPage('error', 'Your account has been closed!');
			break;
		case '3':
			this.setTextPage('error', 'Your account has not been validated yet, please check your e-mail.');
			break;
		case '4':
			if (this.state < 2) {
				this.setTextPage('error', 'The server just did something it\'s not supposed to do. Ever.');
			} else {
				this.gameDisplay.hide();
				this.setTextPage('errsel', 'You are no longer registered to this game!');
			}
			break;
		}
	},

	serverMaintenance: function (reason, until, now) {
		var str = '<b>The server is undergoing maintenance.</b><br/><br/>Reason: ' + reason
			+ '<br/><br/>It will be down until <b>' + until + '</b>.<br/>Current server time: ' + now;
		this.setTextPage('error', str);
	},

	loaderDestroyed: function () {
		this.loader = null;
	},

	dataLoaded: function (data) {
		if (this.state == 0) {
			// We were not loging in, save password if needed
			if (LWWidget.main.getPreference('lwRemAuth') == '1') {
				LWWidget.main.setPreference('lwPassword', LWWidget.main.password);
			}

			this.handleGameList(data);
		} else if (this.state == 1) {
			// We were logged in but asked to select a game
			this.handleGameList(data, true);
		} else if (this.state == 2) {
			// We just received game data for the first time
			this.gameDisplay.setGame(this.currentGame.version, this.currentGame.path);
			this.gameDisplay.handleData(data);
			this.page = this.gameDisplay;
			this.menu.setMode('std');
			this.menu.setSelected('player');
			this.page.draw();
			this.state = 3;
		} else {
			// Update game contents
			this.gameDisplay.handleData(data);
		}
	}

});

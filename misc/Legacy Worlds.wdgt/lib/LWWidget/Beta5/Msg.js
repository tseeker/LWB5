LWWidget.Beta5.Msg = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Communications')[0].getChildren('Messages')[0];

		// Get default folders
		var _f = _d.getChildren('DefaultFolder');
		for (var i in _f) {
			var _a = [parseInt(_f[i].getAttribute('all'), 10), parseInt(_f[i].getAttribute('new'), 10)];
			switch (_f[i].getAttribute('id')) {
				case 'IN': this.inbox = _a; break;
				case 'INT': this.internal = _a; break;
				case 'OUT': this.outbox = _a; break;
			}
		}

		// Get custom folders
		this.custom = new Base.Util.Hashtable();
		_f = _d.getChildren('CustomFolder');
		for (var i in _f) {
			this.custom.put(_f[i].getAttribute('id'), [parseInt(_f[i].getAttribute('all'), 10), parseInt(_f[i].getAttribute('new'), 10), _f[i].getText()]);
		}
	},

	show: function () {
		this.dCustom = false;
	},

	draw: function (_e) {
		var _s = '<p style="text-align:center">';
		_s += this.folderCode('Inbox', this.inbox[0], this.inbox[1]) + '<br/>';
		_s += this.folderCode('Transmissions', this.internal[0], this.internal[1]) + '<br/>'
		_s += this.folderCode('Sent messages', this.outbox[0], this.outbox[1]) + '<br/><br/>';

		if (this.custom.isEmpty()) {
			_s += 'No custom folders';
		} else if (this.dCustom) {
			_s += '<a href="#" onclick="Base.Comp.get(' + this._cid + ').switchCustom();return false">Hide custom folders</a><br/>';
			var cf = this.custom.values();
			for (var i in cf) {
				_s += this.folderCode(cf[i][2], cf[i][0], cf[i][1]) + '<br/>';
			}
		} else {
			_s += '<a href="#" onclick="Base.Comp.get(' + this._cid + ').switchCustom();return false">Display custom folders</a>';
		}

		_s += '</p>';
		_e.innerHTML = _s;
	},

	folderCode: function (name, a, n) {
		var _s = '<b>' + name + '</b>: ';
		if (a) {
			if (n) {
				_s += '<b>' + Base.Util.formatNumber(n) + '</b> new message' + (n > 1 ? 's' : '');
			} else {
				_s += 'no new messages';
			}
			_s += ' (<b>' + Base.Util.formatNumber(a) + '</b> total)';
		} else {
			_s += 'no messages';
		}
		return _s;
	},

	switchCustom: function () {
		this.dCustom = !this.dCustom;
		this.game.draw();
	}

});

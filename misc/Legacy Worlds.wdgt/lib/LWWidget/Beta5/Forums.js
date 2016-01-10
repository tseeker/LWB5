LWWidget.Beta5.Forums = LWWidget.Game.Page.inherits({

	switchMode: function () {
		this.details = !this.details;
		this.game.draw();
	},

	setData: function (data) {
		var _f = data.getChildren('Communications')[0].getChildren('Forums')[0];

		// Read general forums
		var gf = _f.getChildren('GeneralForums'), gcats = new Base.Util.Hashtable();
		for (var i in gf) {
			var c = gf[i], desc = c.getChildren('Description')[0].getText(),
				cf = c.getChildren('Forum'), forums = new Array();
			for (var j in cf) {
				forums.push([cf[j].getText(), parseInt(cf[j].getAttribute('topics'), 10), parseInt(cf[j].getAttribute('unread'), 10)]);
			}
			gcats.put(desc, forums);
		}

		// Read alliance forums
		var af = _f.getChildren('AllianceForums'), aForums = new Array();
		if (af.length) {
			af = af[0].getChildren('Forum');
			for (var i in af) {
				aForums.push([af[i].getText(), parseInt(af[i].getAttribute('topics'), 10), parseInt(af[i].getAttribute('unread'), 10)]);
			}
		}

		// Store data
		this.general = gcats;
		this.alliance = aForums;
	},


	show: function () {
		this.details = false;
	},

	draw: function (_e) {
		if (this.details) {
			_e.innerHTML = this.getDetailedHTML();
		} else {
			_e.innerHTML = this.getShortHTML();
		}
	},


	getShortHTML: function () {
		var text = new Array(), gk = this.general.keys();
		for (var i in gk) {
			text.push(this.catSummary(gk[i], this.general.get(gk[i])));
		}

		if (this.alliance.length) {
			text.push(this.catSummary('Alliance forums', this.alliance));
		}

		return '<p style="text-align:center">' + text.join('<br/>') + '<br/><br/><a href="#" onclick="Base.Comp.get('
			+ this._cid + ').switchMode();return false">Show details</a>';
	},

	catSummary: function (name, list) {
		var s = '<u>' + name + '</u>: ';

		var unread = this.countUnread(list);
		if (unread > 0) {
			s += '<b>' + Base.Util.formatNumber(unread) + '</b> unread topic' + (unread > 1 ? 's' : '');
		} else {
			s += 'no unread topics';
		}
		return s;
	},

	countUnread: function (forums) {
		var s = 0;
		for (var i in forums) {
			s += forums[i][2];
		}
		return s;
	},


	getDetailedHTML: function () {
		var text = new Array(), gk = this.general.keys();

		for (var i in gk) {
			text.push(this.catDetailed(gk[i], this.general.get(gk[i])));
		}

		if (this.alliance.length) {
			text.push(this.catDetailed('Alliance forums', this.alliance));
		}

		return '<p style="text-align:center"><a href="#" onclick="Base.Comp.get('
			+ this._cid + ').switchMode();return false">Hide details</a><br/><br/>' + text.join('<br/><br/>');
	},

	catDetailed: function (name, forums) {
		if (!forums.length) {
			return "";
		}

		var text = new Array();

		text.push('<b>' + name + '</b>');
		for (var i in forums) {
			var f = forums[i], s = '<u>' + f[0] + '</u>: ';

			if (f[1]) {
				s += '<b>' + Base.Util.formatNumber(f[1]) + '</b> topic' + (f[1] > 1 ? 's' : '');
				if (f[2]) {
					s += ' (<b>' + Base.Util.formatNumber(f[2]) + '</b> unread)';
				}
			} else {
				s += 'empty forum';
			}
			text.push(s);
		}

		return text.join('<br/>');
	}

});

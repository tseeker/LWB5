LWWidget.Beta5.Player = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Player')[0];

		// Player name and cash
		this.playerName = _d.getAttribute('name');
		this.cash = _d.getAttribute('cash');

		// Alliance status
		var _a = _d.getChildren('Alliance');
		if (_a.length) {
			_a = _a[0];
			this.alliance = { joined: (_a.getAttribute('inalliance') == '1'), tag: _a.getAttribute('tag') };
		} else {
			this.alliance = null;
		}

		// General rankings
		_a = _d.getChildren('Rankings')[0];
		var _r = _a.getChildren('Ranking');
		for (var i in _r) {
			if (_r[i].getText() != 'General Ranking') {
				continue;
			}
			this.points = _r[i].getAttribute('points');
			this.rank = _r[i].getAttribute('rank');
			break;
		}

		// Planets / planets under attack
		_d = data.getChildren('Empire')[0].getChildren('Planets')[0];
		this.planets = parseInt(_d.getAttribute('count'), 10);
		this.underAttack = parseInt(_d.getAttribute('siege'), 10);

		// Fleets / fleets in battle
		_d = data.getChildren('Empire')[0].getChildren('Fleets')[0];
		this.fleets = parseInt(_d.getAttribute('count'), 10);
		this.inBattle = parseInt(_d.getAttribute('inBattle'), 10);
	},

	draw: function (_e) {
		if (!this.playerName) {
			return;
		}

		var str = '<p style="text-align:center">Player <b>' + this.playerName + '</b>';
		if (this.alliance && this.alliance.joined) {
			str += ' [<b>' + this.alliance.tag + '</b>]';
		} else if (this.alliance && !this.alliance.joined) {
			str += '<br/>Requesting to join alliance [<b>' + this.alliance.tag + '</b>]';
		}

		str += '<br/><br/>Cash: <b>&euro;' + Base.Util.formatNumber(this.cash) + '</b>'
			+ '<br/>General ranking: <b>#' + Base.Util.formatNumber(this.rank) + '</b> (<b>'
			+ Base.Util.formatNumber(this.points) + '</b> points)<br/><br/>';

		if (this.planets == 0) {
			str += "No planets";
		} else {
			str += '<b>' + Base.Util.formatNumber(this.planets) + '</b> planet' + (this.planets > 1 ? 's' : '');
			if (this.underAttack > 0) {
				str += ' (<b>' + Base.Util.formatNumber(this.underAttack) + '</b> under attack!)';
			}
		}
		str += '<br/>';

		if (this.fleets == 0) {
			str += "No fleets";
		} else {
			str += '<b>' + Base.Util.formatNumber(this.fleets) + '</b> fleet' + (this.fleets > 1 ? 's' : '');
			if (this.inBattle > 0) {
				str += ' (<b>' + Base.Util.formatNumber(this.inBattle) + '</b> in battle)';
			}
		}

		str += '</p>';

		_e.innerHTML = str;
	}

});

LWWidget.Beta5.Fleets = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Empire')[0].getChildren('Fleets')[0];

		this.fleets = parseInt(_d.getAttribute('count'), 10);
		if (this.fleets) {
			// Fleets in battle
			this.inBattle = parseInt(_d.getAttribute('inBattle'), 10);
			// Total fleet power
			this.power = parseInt(_d.getAttribute('power'), 10);
			// Ships types
			this.gaships = parseInt(_d.getAttribute('gaships'), 10);
			this.fighters = parseInt(_d.getAttribute('fighters'), 10);
			this.cruisers = parseInt(_d.getAttribute('cruisers'), 10);
			this.bcruisers = parseInt(_d.getAttribute('bcruisers'), 10);
			// Upkeep
			_d = data.getChildren('Empire')[0].getChildren('Budget')[0];
			this.upkeep = parseInt(_d.getAttribute('upkeep'), 10);
		}
	},

	draw: function (_e) {
		if (this.fleets == 0) {
			_e.innerHTML = '<p style="text-align:center">No fleets</p>';
			return;
		}

		var _s = '<p style="text-align:center"><b>' + Base.Util.formatNumber(this.fleets)
			+ '</b> fleet' + (this.fleets > 1 ? 's' : '');
		if (this.inBattle) {
			_s += ' (<b>' + Base.Util.formatNumber(this.inBattle) + '</b> engaged in battle!)';
		}
		_s += '<br/><br/>Total fleet power: <b>' + Base.Util.formatNumber(this.power)
			+ '</b><br/>Upkeep: <b>&euro;' + Base.Util.formatNumber(this.upkeep)
			+ '</b><br/><br/><u>Ship types</u><br/>';

		if (this.gaships) {
			_s += 'G.A. ships: <b>' + Base.Util.formatNumber(this.gaships) + '</b><br/>';
		}
		if (this.fighters) {
			_s += 'Fighters: <b>' + Base.Util.formatNumber(this.fighters) + '</b><br/>';
		}
		if (this.cruisers) {
			_s += 'Cruisers: <b>' + Base.Util.formatNumber(this.cruisers) + '</b><br/>';
		}
		if (this.bcruisers) {
			_s += 'Battle Cruisers: <b>' + Base.Util.formatNumber(this.bcruisers) + '</b><br/>';
		}
		_s += '</p>';

		_e.innerHTML = _s;
	}

});

LWWidget.Beta5.Planets = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Empire')[0].getChildren('Planets')[0];

		this.planets = parseInt(_d.getAttribute('count'), 10);
		this.pList = new Array();
		if (this.planets) {
			// Planets under attack
			this.underAttack = parseInt(_d.getAttribute('siege'), 10);
			// Happiness / Corruption
			this.avgHappiness = parseInt(_d.getAttribute('avgHap'), 10);
			this.avgCorruption = parseInt(_d.getAttribute('avgCor'), 10);
			// Population
			this.totPopulation = parseInt(_d.getAttribute('totPop'), 10);
			this.avgPopulation = parseInt(_d.getAttribute('avgPop'), 10);
			// Factories
			this.totFactories = parseInt(_d.getAttribute('totFac'), 10);
			this.avgFactories = parseInt(_d.getAttribute('avgFac'), 10);
			// Factories
			this.totTurrets = parseInt(_d.getAttribute('totTur'), 10);
			this.avgTurrets = parseInt(_d.getAttribute('avgTur'), 10);

			// Planet list
			var _l = _d.getChildren('List')[0].getChildren('Planet');
			for (var i in _l) {
				this.pList.push(_l[i].getText());
			}
		}
	},

	show: function () {
		this.mode = 0;
	},

	draw: function (_e) {
		_e.innerHTML = (this.mode == 0 || !this.planets) ? this.getStats() : this.getList();
	},

	setMode: function (m) {
		this.mode = m;
		if (this.game) {
			this.game.draw();
		}
	},


	getStats: function () {
		if (this.planets == 0) {
			return '<p style="text-align:center">No planets.</p>';
		}

		var _s = '<p style="text-align:center"><b>' + Base.Util.formatNumber(this.planets) + '</b> planet'
			+ (this.planets > 1 ? 's' : '') + '<br/>';
		if (this.underAttack) {
			_s += '<b>' + Base.Util.formatNumber(this.underAttack) + ' planet'
				+ (this.underAttack > 1 ? 's' : '') + ' under attack!</b>';
		} else {
			_s += 'No planets under attack';
		}

		_s += '<br/><br/>Average happiness: <b>' + this.avgHappiness + '%</b><br/>Average corruption: <b>'
			+ this.avgCorruption + '%</b><br/>Population: <b>' + Base.Util.formatNumber(this.totPopulation) + '</b>';
		if (this.planets > 1) {
			_s += ' (<b>'  + Base.Util.formatNumber(this.avgPopulation) + '</b> on average)';
		}
		_s += '<br/>Factories: <b>' + Base.Util.formatNumber(this.totFactories) + '</b>';
		if (this.planets > 1) {
			_s += ' (<b>'  + Base.Util.formatNumber(this.avgFactories) + '</b> on average)';
		}
		_s += '<br/>Turrets: <b>' + Base.Util.formatNumber(this.totTurrets) + '</b>';
		if (this.planets > 1) {
			_s += ' (<b>'  + Base.Util.formatNumber(this.avgTurrets) + '</b> on average)';
		}
		_s += '<br/><br/><a href="#" onclick="Base.Comp.get(' + this._cid + ').setMode(1);return false">View list</a></p>';

		return _s;
	},

	getList: function () {
		return '<p style="text-align:center"><a href="#" onclick="Base.Comp.get(' + this._cid
			+ ').setMode(0);return false">&lt;&lt;- Back</a><br/><br/><b>'
			+ this.pList.join('</b><br/><b>') + '</b></p>';
	}

});

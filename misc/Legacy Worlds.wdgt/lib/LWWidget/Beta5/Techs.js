LWWidget.Beta5.Techs = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Empire')[0].getChildren('Research')[0];

		this.points = parseInt(_d.getAttribute('points'), 10);
		this.newTechs = parseInt(_d.getAttribute('new'), 10);
		this.foreseen = parseInt(_d.getAttribute('foreseen'), 10);

		_d = _d.getChildren('RBudget')[0];
		this.fundamental = parseInt(_d.getAttribute('fundamental'), 10);
		this.military = parseInt(_d.getAttribute('military'), 10);
		this.civilian = parseInt(_d.getAttribute('civilian'), 10);
	},

	draw: function (_e) {
		var _s = '<p style="text-align:center">';
		if (this.newTechs == 0) {
			_s += 'No new technologies discovered.';
		} else {
			_s += '<b>' + this.newTechs + '</b> new technolog' + (this.newTechs > 1 ? 'ies' : 'y') + ' discovered.';
		}
		_s += '<br/>';
		if (this.foreseen == 0) {
			_s += 'No breakthroughs foreseen.';
		} else {
			_s += '<b>' + this.foreseen + '</b> breakthrough' + (this.foreseen > 1 ? 's' : '') + ' foreseen.';
		}
		_s += '<br/><br/><u>Research budget</u><br/><br/><b>' + Base.Util.formatNumber(this.points) + '</b> research points per day<br/><b>'
			+ this.fundamental + '%</b> allocated to Fundamental research<br/><b>'
			+ this.military + '%</b> allocated to Military research<br/><b>'
			+ this.civilian + '%</b> allocated to Civilian research</p>';

		_e.innerHTML = _s;
	}

});

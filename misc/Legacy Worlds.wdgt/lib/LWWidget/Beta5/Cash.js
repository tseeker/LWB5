LWWidget.Beta5.Cash = LWWidget.Game.Page.inherits({

	setData: function (data) {
		var _d = data.getChildren('Empire')[0].getChildren('Budget')[0];

		this.income = parseInt(_d.getAttribute('income'), 10);
		this.upkeep = parseInt(_d.getAttribute('upkeep'), 10);
		this.profit = parseInt(_d.getAttribute('profit'), 10);
	},

	draw: function (_e) {
		_e.innerHTML = '<p style="text-align:center"><br/><br/>Planetary income: <b>&euro;' + Base.Util.formatNumber(this.income)
			+ '</b><br/><br/>Fleet upkeep: <b>&euro;' + Base.Util.formatNumber(this.upkeep)
			+ '</b><br/><br/>Daily profit: <b>&euro;' + Base.Util.formatNumber(this.profit) + '</p>';
	}

});

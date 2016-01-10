LWWidget.GameDisplay = LWWidget.Page.inherits({

	constructor: function () {
		this.base();

		this.addSlot('gameDestroyed');
		this.addEvent('Refresh');

		this.gPath = '';
		this.game = null;
	},

	destroy: function () {
		if (this.game) {
			this.game.destroy();
		}
		this.base();
	},


	setGame: function (gClass, gPath) {
		if (this.game) {
			this.game.destroy();
		}
		this.game = new gClass(gPath);
		this.game.bindEvent('Destroy', 'gameDestroyed', this);
		this.game.bindEvent('RequestUpdate', 'onRefresh', this);
	},

	gameDestroyed: function () {
		this.game = null;
	},


	handleData: function (data) {
		if (this.game) {
			this.game.setData(data);
		}
	},


	hide: function () {
		if (this.game) {
			this.game.hide();
		}
	},


	show: function () {
		if (this.game) {
			this.game.show();
		}
		this.draw();
	},


	setPage: function (pName) {
		if (this.game) {
			this.game.setPage(pName);
		}
	},


	draw: function () {
		if (!document.getElementById('gDisplay')) {
			this.writeContents('<div id="gDisplay" style="overflow:auto;width:100%;height:100%">&nbsp;</div>');
		}
		if (this.game) {
			this.game.draw();
		}
	}

});

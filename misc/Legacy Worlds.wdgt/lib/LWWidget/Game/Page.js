LWWidget.Game.Page = Base.Comp.inherits({

	constructor: function (game) {
		this.base();

		this.addSlot('gameDestroyed');
		this.game = game;
		this.game.bindEvent('Destroy', 'gameDestroyed', this);
	},

	writeContents: function (contents) {
		var e = document.getElementById('gDisplay');
		if (e) {
			e.innerHTML = contents;
		}
	},

	gameDestroyed: function () {
		this.game = null;
	},

	setData: function (data) {
	},

	draw: function (_e) {
	},

	show: function () {
	},

	hide: function () {
	}

});

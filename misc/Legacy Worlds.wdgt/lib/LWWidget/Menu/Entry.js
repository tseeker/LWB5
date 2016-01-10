LWWidget.Menu.Entry = LWWidget.Menu.Item.inherits({

	constructor: function (name, text) {
		this.base(name, text, true, true, ' - ');
	}

});

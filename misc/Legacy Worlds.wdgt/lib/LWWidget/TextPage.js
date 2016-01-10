LWWidget.TextPage = LWWidget.Page.inherits({

	constructor: function () {
		this.text = "";
	},

	setText: function (text) {
		if (this.text != text) {
			this.text = text;
		}
		this.draw();
	},

	draw: function () {
		this.writeContents(this.text == "" ? '&nbsp;' : this.text);
	}

});

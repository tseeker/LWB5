LWWidget.Menu.Mode = Base.Comp.inherits({

	constructor: function (name) {
		this.base();

		this.name = name;
		this.items = new Base.Util.Hashtable();
		this.selected = null;

		this.addSlot('itemDestroyed');
		this.addSlot('itemClicked');
		this.addEvent('Click');
	},

	destroy: function () {
		var items = this.items.values();
		for (var i in items) {
			var _i = items[i];
			this.items.remove(items[i]._cid);
			items[i].destroy();
		}

		this.base();
	},

	draw: function () {
		var str = '', items = this.items.values();

		for (var i=0;i<items.length;i++) {
			str += items[i].draw();
			if (i < items.length - 1 && items[i].separator) {
				str += items[i].separator;
			}
		}

		return str;
	},


	addItem: function (item) {
		if (!(item && item._cid) || this.items.containsKey(item._cid)) {
			return;
		}
		this.items.put(item._cid, item);
		if (item.selectable && !this.selected) {
			item.selected = true;
			this.selected = item;
		} else {
			item.selected = false;
		}

		item.bindEvent('Destroy', 'itemDestroyed', this);
		item.bindEvent('Click', 'itemClicked', this);
	},


	setSelected: function (name) {
		var _k = this.items.values();
		for (var i in _k) {
			var item = _k[i];
			if (item.selectable && item.name == name) {
				if (this.selected) {
					if (this.selected._cid == item._cid) {
						return;
					}
					this.selected.selected = false;
				}
				item.selected = true;
				this.selected = item;
				return;
			} else if (item.name == name) {
				return;
			}
		}
	},


	itemClicked: function (item) {
		if (item.selectable) {
			if (this.selected) {
				this.selected.selected = false;
			}
			item.selected = true;
			this.selected = item;
		}
		this.onClick(item.name);
	},

	itemDestroyed: function (item) {
		if (!(item && item._cid && this.items.containsKey(item._cid))) {
			return;
		}
		this.items.remove(item._cid);

		if (this.selected && this.selected._cid == item._cid) {
			var _v = this.items.values(), _s = null;
			for (var i in _v) {
				var _i = _v[i];
				if (_i.selectable) {
					_s = _i;
					_i.selected = true;
					break;
				}
			}
			this.selected = _s;
		}
	}

});

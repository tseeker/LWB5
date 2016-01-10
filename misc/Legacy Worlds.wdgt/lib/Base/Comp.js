/** The Base.Comp class is the base class for all of the components. It provides the
 * signal/slot framework that the components use, and allows access to components using
 * their unique identifier.
 */
Base.Comp = Base.inherits({

	/** The constructor initialises the component by assigning its identifier, creating the
	 * tables listing the slots, events and children, and creating the low-level slots and
	 * events common to all components.
	 */
	constructor: function () {
		this._cid	= ++ Base.Comp.lastId;
		this._slots	= new Base.Util.Hashtable();
		this._evts	= new Base.Util.Hashtable();
		this._chld	= new Base.Util.Hashtable();
		this._cnt	= null;

		Base.Comp.list.put(this._cid, this);
		Base.Comp.count ++;

		this.destroyChildren = false;

		this.addEvent('Destroy');
		this.addSlot('addChild');
		this.addSlot('removeChild');
	},

	/** This method is responsible for the destruction of a component. It
	 * starts by launching the onDestroy() event; it then detaches the
	 * component's children (or destroy them if the destroyChildren member
	 * is set to true), then detaches all of the component's slots and
	 * events. Finally, it removes the reference to the component in the
	 * global components list.
	 */
	destroy: function () {
		this.onDestroy(this._cid);

		var k = this._chld.keys();
		for (var i in k) {
			var c = this._chld.get(k[i]);
			if (this.destroyChildren) {
				c.destroy();
			} else {
				this.removeChild(c);
			}
		}

		k = this._slots.keys();
		for (var i in k) {
			this._slots.get(k[i]).destroy();
		}
		this._slots.clear();

		k = this._evts.keys();
		for (var i in k) {
			this._evts.get(k[i]).destroy();
		}
		this._evts.clear();

		Base.Comp.list.remove(this._cid);
		Base.Comp.count --;

		for (var i in this) {
			this[i] = null;
		}
	},


	/** This method declares a new slot for the current component.
	 * @param	name	the name of the slot to be declared
	 */
	addSlot: function (name) {
		if (this._slots.containsKey(name) || !this[name]) {
			return;
		}
		this._slots.put(name, new Base.Comp.Slot(name, this));
	},

	/** This method generates a new event handler for the current component.
	 * @param	name		the name of the event to create
	 * @param	propagate	a boolean indicating the event propagates to the component's children
	 */
	addEvent: function (name, propagate) {
		if (this._evts.containsKey(name) || this["on" + name]) {
			return;
		}

		this._evts.put(name, new Base.Comp.Evt(name, propagate));
		this["on" + name] = function() {
			this.triggerEvent(name, arguments);
		};
		this.addSlot('on' + name);
	},


	/** This method binds an event from the current component to a slot on either the current
	 * component or, if the last parameter is present, another component.
	 * @param	eName		the name of the event to bind
	 * @param	sName		the name of the slots to bind an event to
	 * @param	cmp		the component on which to bind (if this parameter isn't passed, the current component is used)
	 */
	bindEvent: function (eName, sName, cmp) {
		var e = this._evts.get(eName), c = (cmp ? cmp : this), s = c._slots.get(sName);
		if (!(e && s)) {
			return;
		}
		e.bind(s);
	},

	/** This method detaches an event from the current component from a slot to which it had been bound.
	 * @param	eName		the name of the event to detach
	 * @param	sName		the name of the slot to detach
	 * @param	cmp		the component on which the slot to detach is located
	 */
	detachEvent: function (eName, sName, cmp) {
		var e = this._evts.get(eName), c = (cmp ? cmp : this), s = c._slots.get(sName);
		if (!(e && s)) {
			return;
		}
		e.detach(s);
	},


	/** This method triggers the execution of an event on the current component.
	 * @param	eName		the name of the event to trigger
	 * @param	args		an array containing the arguments to be passed to the event's handlers
	 */
	triggerEvent: function (eName, args) {
		var e = this._evts.get(eName);
		if (!e) {
			return;
		}

		e.trigger.apply(e, args);

		if (!e.propagate) {
			return;
		}

		var k = this._chld.keys();
		for (i=0;i<k.length;i++) {
			this._chld.get(k[i]).triggerEvent(eName, args);
		}
	},


	/** This method adds a child to the current component.
	 * @param	cmp	the component to add as a child of the current component
	 */
	addChild: function (cmp) {
		if (!cmp || cmp._cid == this._cid || cmp.container != null) {
			return;
		}
		cmp.container = this;
		this._chld.put(cmp._cid, cmp);
		cmp.bindEvent('Destroy', 'removeChild', this);
	},

	/** This method removes a child from the current component's list of children.
	 * @param	cmp	the child component to remove
	 */
	removeChild: function (cmp) {
		if (!(cmp && this._chld.containsKey(cmp._cid))) {
			return;
		}
		this._chld.remove(cmp._cid);
		cmp.container = null;
		cmp.detachEvent('Destroy', 'removeChild', this);
	},

	/** This method checks whether a component is a child of the current component.
	 * @param	cmp	the component to verify
	 * @returs		true if the specified component is a child of the current component, false otherwise
	 */
	hasChild: function (cmp) {
		return (cmp && cmp.container && cmp.container._cid == this._cid);
	}

}, {

	/** This class property contains the list of all available components;
	 * the components' identifiers are used as the key.
	 */
	list: new Base.Util.Hashtable(),

	/** This class property stores the last identifier assigned to a
	 * component.
	 */
	lastId:	-1,

	/** This class property stores the amount of components currently
	 * being used.
	 */
	count: 0,

	/** This static method returns a component from its identifier.
	 * @param	id	the component's identifier
	 */
	get: function (id) {
		if (!Base.Comp.list.containsKey(id)) {
			return null;
		}
		return Base.Comp.list.get(id);
	},

	/** This static method destroys all of the components.
	 */
	onUnload: function () {
		var l = Base.Comp.list.values();
		for (var i in l) {
			if (l[i] && l[i].destroy) {
				l[i].destroy();
			}
		}
		Base.Comp.list.clear();
	}
});

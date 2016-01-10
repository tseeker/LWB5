/** This class defines the data structure and operations required to handle a component's
 * slots.
 */
Base.Comp.Slot = Base.inherits({

	/** The constructor generates a new slot bound to a specified component.
	 * @param	name		the slot's name
	 * @param	component	the component instance to which this slot belongs
	 */
	constructor: function (name, component) {
		this.id		= ++Base.Comp.Slot.lId;
		this.name	= name;
		this.component	= component;
		this.events	= new Base.Util.Hashtable();
	},

	/** This method calls the component's method for the slot the current instance
	 * represents.
	 * @param	a	an array containing the parameters to be passed to the
	 *				component's method.
	 */
	call: function(a) {
		this.component[this.name].apply(this.component, a);
	},

	/** This method destroys a slot.
	 */
	destroy: function () {
		var k = this.events.keys(), i;
		for (i=0;i<k.length;i++) {
			this.events.get(k[i]).detach(this);
		}

		Base.Comp.Slot.list[this.id] = null;
		for (var i in this) {
			this[i] = null;
		}
	}

}, {

	/** This class property contains the last attributed slot identifier.
	 */
	lId: -1,

	/** This class property contains the list of all existing slots.
	 */
	list: new Array()

});

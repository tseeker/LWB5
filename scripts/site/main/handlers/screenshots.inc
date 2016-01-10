<?php

class page_handler {

	static private $screenshots = array(
		"b5"	=> array(
			'title'	=> 'Beta 5',
			'pics'	=> array(
				'ov'		=> 'Overview',
				'planets'	=> 'Planet list',
				'planet'	=> 'Individual planet page',
				'fleets'	=> 'Fleets management',
				'research'	=> 'Research',
				'money'		=> 'Money',
				'map'		=> 'Maps',
				'messages'	=> 'Messages',
				'ranking'	=> 'Rankings',
				'allies'	=> 'Trusted allies',
				'market'	=> 'Marketplace',
				'manual'	=> 'In-game manual'
			)
		),
		"b4"	=> array(
			'title'	=> 'Beta 4',
			'pics'	=> array(
				'ov'		=> 'Overview',
				'planets'	=> 'Planet list',
				'fleets'	=> 'Fleets management',
				'money'		=> 'Money',
				'map'		=> 'Maps',
				'ticks'		=> 'Ticks',
				'ranking'	=> 'Rankings'
			)
		),
	);


	public function handle($input) {
		$this->data = array(
			'list'	=> self::$screenshots
		);
		if ($input['c'] && array_key_exists($input['c'], self::$screenshots)) {
			$cat = self::$screenshots[$input['c']];
			$this->data['category'] = $input['c'];
			if ($input['s'] && array_key_exists($input['s'], $cat['pics'])) {
				$this->data['picture'] = $input['s'];
			}
		}

		$this->output = "screenshots";
	}

}

?>

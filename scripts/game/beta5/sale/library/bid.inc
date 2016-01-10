<?php

class beta5_sale_bid {
	var $index = array();


	function beta5_sale_bid($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
	}


	function run($pid, $oid, $price) {
		$q = $this->db->query(
			"SELECT id,price,player FROM sale,public_offer"
			." WHERE id=$oid AND offer=$oid AND player<>$pid AND auction"
			. " AND finalized IS NULL AND expires>UNIX_TIMESTAMP(NOW())"
		);
		if (!($q && dbCount($q))) {
			return "0";
		}
		$r = dbFetchArray($q);
		$q = $this->db->query("SELECT price FROM auction WHERE offer=$oid ORDER BY price DESC LIMIT 1");
		if ($q && dbCount($q)) {
			list($mp) = dbFetchArray($q);
		} else {
			$mp = $r[1];
		}
		if ($mp >= $price) {
			return "2#$oid#$mp";
		}

		$q = $this->db->query("SELECT price FROM auction WHERE offer=$oid AND player=$pid ORDER BY price DESC LIMIT 1");
		if ($q && dbCount($q)) {
			list($lBid) = dbFetchArray($q);
		} else {
			$lBid = 0;
		}
		$rPrice = $price - $lBid;

		$pi = $this->players->call('get', $pid);
		if ($pi['cash'] < $rPrice) {
			return "1";
		}
		$this->db->query("UPDATE player SET cash=cash-$rPrice WHERE id=$pid");
		$this->db->query("INSERT INTO auction VALUES ($oid,$pid,".time().",$price)");
		$this->sendOwnerBidMessage($r[2], $oid, $price, $pid);

		$q = $this->db->query("SELECT player FROM auction WHERE offer=$oid AND price<$price ORDER BY price DESC LIMIT 1");
		if (!($q && dbCount($q))) {
			return "";
		}
		list($oldPId) = dbFetchArray($q);
		if ($oldPId == $pid) {
			return "";
		}
		$this->sendNewBidMessage($oldPId, $oid, $price);
		return "";
	}


	function sendOwnerBidMessage($player, $offer, $newPrice, $bidder) {
		$q = $this->db->query("SELECT fleet,planet FROM sale WHERE id=$offer");
		list($fid,$pid) = dbFetchArray($q);
		$qs = "$offer," . (is_null($pid)?0:1);

		if (!is_null($fid)) {
			$fleet = $this->fleets->call('get', $fid);
			$g = $fleet['gaships']; $f = $fleet['fighters']; $c = $fleet['cruisers']; $b = $fleet['bcruisers'];
			if (is_null($pid)) {
				$pid = $fleet['location'];
			}
		} else {
			$g=$f=$c=$b=0;
		}
		$pinf = $this->planets->call('byId', $pid);
		$pname = $pinf['name'];

		$qs .= ",'".addslashes($pname)."',$pid,$g,$f,$c,$b,$newPrice";

		$tm = time();
		$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($player,$tm,'bid','INT',TRUE)");
		$q = $this->db->query("SELECT id FROM message WHERE player=$player AND sent_on=$tm AND mtype='bid' ORDER BY id DESC LIMIT 1");
		list($mid) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_bid VALUES($mid,$qs,$bidder)");
	}


	function sendNewBidMessage($player, $offer, $newPrice) {
		$q = $this->db->query("SELECT fleet,planet FROM sale WHERE id=$offer");
		list($fid,$pid) = dbFetchArray($q);
		$qs = "$offer," . (is_null($pid)?0:1);

		if (!is_null($fid)) {
			$fleet = $this->fleets->call('get', $fid);
			$g = $fleet['gaships']; $f = $fleet['fighters']; $c = $fleet['cruisers']; $b = $fleet['bcruisers'];
			if (is_null($pid)) {
				$pid = $fleet['location'];
			}
		} else {
			$g=$f=$c=$b=0;
		}
		$pinf = $this->planets->call('byId', $pid);
		$pname = $pinf['name'];

		$qs .= ",'".addslashes($pname)."',$pid,$g,$f,$c,$b,$newPrice";

		$tm = time();
		$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($player,$tm,'bid','INT',TRUE)");
		$q = $this->db->query("SELECT id FROM message WHERE player=$player AND sent_on=$tm AND mtype='bid' ORDER BY id DESC LIMIT 1");
		list($mid) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_bid VALUES($mid,$qs,NULL)");
	}
}

?>

// SPDX-License-Identifier: GPL-3.0-or-later

// This file is part of the http://github.com/aaronbloomfield/ccc repository,
// and is released under the GPL 3.0 license.

pragma solidity ^0.8.17;

import "./DEX.sol";
import "./TokenCC.sol";
import "./EtherPriceOracleConstant.sol";
import "./IERC20Receiver.sol";

// See the homework description for how to use this program

contract DEXtest is IERC20Receiver {

    TokenCC public tc;
    DEX public dex;

    constructor() {
        tc = new TokenCC();
        dex = new DEX();
    }

    function onERC20Received(address from, uint amount, address erc20) external returns (bool) {
        // implement as needed; you have to put code in here if you transfer
        // TCC to this contract
    }

	function test() public payable {
 		require (msg.value == 13 ether, "Must call test() with 13 ether");

        // Step 1: deploy the ether price oracle
        IEtherPriceOracle pricer = new EtherPriceOracleConstant();

        // Step 1 tests: DEX is deployed
        require(dex.k() == 0, "k value not 0 after DEX creation()");
        require(dex.x() == 0, "x value not 0 after DEX creation()");
        require(dex.y() == 0, "y value not 0 after DEX creation()");

        // Step 2: createPool() is called with 10 (fake) ETH and 100 TC
        bool success = tc.approve(address(dex),1000000*10**tc.decimals()); //was 100
        require (success,"Failed to approve TC before createPool()");
        try dex.createPool{value: 10 ether}(100*10**tc.decimals(), 0, 1000, address(tc), address(pricer)) {
            // do nothing
        } catch Error(string memory reason) {
            require (false, string.concat("createPool() call reverted: ",reason));
        }
        
        // Step 2 tests
        require(dex.k() == 1e21 * 10**tc.decimals(), "k value not correct after createPool()");
        require(dex.x() == 10 * 1e18, "x value not correct after createPool()");
        require(dex.y() == 100 * 10**tc.decimals(), "y value not correct after createPool()");

        require(address(this).balance == 3 ether, "Balance is greater than 2");
        // Step 3: transaction 1, where 2.5 ETH is provided to the DEX for exchange
        //(success, ) = payable(dex).call{value: 1 ether}(""); //25000000000000
        //require(success, "Failed to pay");


        

        //require(dex.k() == 1e21 * 10**tc.decimals(), "k value not correct after receive");
        //require(dex.x() == (12 * 1e18), "x value not correct after receive()");
        //require(dex.y() == 80 * 10**tc.decimals(), "y value not correct after receive()");

        // // Step 3 tests. DONE ABOVE!

        // // Step 4: transaction 2, where 120 TC is provided to the DEX for exchange
        // dex.onERC20Received(msg.sender, 120*10**tc.decimals(), address(dex));
        //     // do nothing
        

        // require(dex.k() == 1e21 * 10**tc.decimals(), "k value not correct after erc20 recv");
        // require(dex.x() == (5 * 1e18), "x value not correct after erc20 recv()");
        // require(dex.y() == 200 * 10**tc.decimals(), "y value not correct after erc20 recv()");
  
        // Step 4 tests

        // Step 5: addLiquidity() is called with 1 (fake) ETH and 40 TC

        // Step 5 tests

        // finish up
        //require(false,"end fail"); // huh?  see why in the homework description!
    }
 
    receive() external payable { } // see note in the HW description

}
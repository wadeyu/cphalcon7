<?php 

namespace Phalcon\Db {

	/**
	 * Phalcon\Db\ResultInterface initializer
	 */
	
	interface ResultInterface {

		/**
		 * Allows to executes the statement again. Some database systems don't support scrollable cursors,
		 * So, as cursors are forward only, we need to execute the cursor again to fetch rows from the begining
		 *
		 * @return boolean
		 */
		public function execute();


		/**
		 * Fetches an array/object of strings that corresponds to the fetched row, or FALSE if there are no more rows.
		 * This method is affected by the active fetch flag set using \Phalcon\Db\Result\Pdo::setFetchMode
		 *
		 * @return mixed
		 */
		public function fetch($fetchMode=null, $fetchArgument=null, $ctorArgs=null);


		/**
		 * Returns an array of strings that corresponds to the fetched row, or FALSE if there are no more rows.
		 * This method is affected by the active fetch flag set using \Phalcon\Db\Result\Pdo::setFetchMode
		 *
		 * @return mixed
		 */
		public function fetchArray();


		/**
		 * Returns an array of arrays containing all the records in the result
		 * This method is affected by the active fetch flag set using \Phalcon\Db\Result\Pdo::setFetchMode
		 *
		 * @return array
		 */
		public function fetchAll();


		/**
		 * Gets number of rows returned by a resulset
		 *
		 * @return int
		 */
		public function numRows();


		/**
		 * Moves internal resulset cursor to another position letting us to fetch a certain row
		 *
		 * @param int $number
		 */
		public function dataSeek($number);


		/**
		 * Changes the fetching mode affecting \Phalcon\Db\Result\Pdo::fetch()
		 *
		 * @param int $fetchMode
		 */
		public function setFetchMode($fetchMode, $fetchArg1=null, $fetchArg2=null);


		/**
		 * Gets the internal PDO result object
		 *
		 * @return \PDOStatement
		 */
		public function getInternalResult();

	}
}

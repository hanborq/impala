/*
 * hdfs-hfile-scanner.h
 *
 *  Created on: 2013-5-9
 *      Author: Rao
 */

#ifndef HDFS_HFILE_SCANNER_H_
#define HDFS_HFILE_SCANNER_H_

#include "exec/hdfs-scanner.h"
#include "exec/hdfs-scan-node.h"
#include "exec/hfile-types.h"
#include "util/codec.h"
#include <boost/scoped_ptr.hpp>

namespace impala
{

class RuntimeState;
class MemPool;
class Status;

//Scanner used to parse HFile file format.

class HdfsHFileScanner: public HdfsScanner
{
public:
	HdfsHFileScanner(HdfsScanNode* scan_node, RuntimeState* state);
	virtual ~HdfsHFileScanner();

	// Issue io manager byte ranges for 'files'
	static void IssueInitialRanges(HdfsScanNode*,const std::vector<HdfsFileDesc*>& files);

	// Implementation of HdfsScanner interface.
	virtual Status Prepare();
	virtual Status ProcessSplit(ScannerContext* context);
	virtual Status Close();

//	static Status Init();

private:

	Status ProcessTrailer();
	Status ReadDataBlock();
	Status IssueFileRanges(const char* filename);
	bool WriteTuple(MemPool* pool, Tuple* tuple,bool skip);
	Status ProcessSplitInternal();


	boost::scoped_ptr<MemPool> decompressed_data_pool_;
        boost::scoped_ptr<Codec> decompressor_;
	//Buffer to hold decompressed data.
	uint8_t* block_buffer_;
	//Allocated length of the block_buffer_;
	int32_t block_buffer_len_;

	uint8_t* byte_buffer_ptr_;
	uint8_t* byte_buffer_end_;
	uint32_t num_checksum_bytes_;

	std::vector<PrimitiveType> col_types_;
	std::vector<PrimitiveType>  key_col_types_;
	std::vector<PrimitiveType>  value_col_types_;
	//in current implementation, there is no easy to figure out the number of columns in key part of a KeyValue object.
	//So we de-serialize a record to get this number.
	int num_key_cols_;
	int num_clustering_cols_;
	class KeyValue;
	boost::scoped_ptr<KeyValue> kv_parser_;
	hfile::FixedFileTrailer* trailer_;
	bool only_parsing_trailer_;

};

} //namespace impala

#endif /* HDFS_HFILE_SCANNER_H_ */

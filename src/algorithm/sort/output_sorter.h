#ifndef TOMAHAWKOUTPUTSORT_H_
#define TOMAHAWKOUTPUTSORT_H_

#include <thread>
#include <queue>

#include "../../io/compression/TGZFEntryIterator.h"
#include "../../tomahawk/two/output_slave_writer.h"
#include "../../tomahawk/two/TomahawkOutputReader.h"
#include "output_sort_merge_queue.h"
#include "output_sort_slave.h"

namespace Tomahawk{
namespace Algorithm{

/**<
 * Primary class for sorting `TWO` data
 */
class OutputSorter{
	typedef IO::OutputEntry                   entry_type;
	typedef IO::TomahawkOutputReader          two_reader_type;
	typedef IO::WriterFile                    basic_writer_type;
	typedef IO::TGZFEntryIterator<entry_type> tgzf_iterator;
	typedef OutputSorter                      self_type;
	typedef OutputSortMergeQueue<entry_type>  queue_entry;
	//typedef OutputSortSlave                   slave_sorter;
	typedef std::priority_queue<queue_entry>  queue_type; // prio queue

public:
	OutputSorter() : n_threads(std::thread::hardware_concurrency()), reverse_entries(true){}
	~OutputSorter(){}

	bool sort(const std::string& input, const std::string& destinationPrefix, U64 memory_limit);
	bool sortMerge(const std::string& input, const std::string& destinationPrefix, const U32 block_size);

	inline const size_t size(void) const{ return(this->n_threads); }
	inline bool isReverseEntries(void) const{ return(this->reverse_entries); }
	inline void setReverseEntries(const bool yes){ this->reverse_entries = yes; }

private:
	bool __sortUnindexed();
	bool __sortIndexed(basic_writer_type& toi_writer, const std::string& input, U64 memory_limit);

private:
	two_reader_type reader;

public:
	size_t      n_threads;
	bool        reverse_entries;
	std::string baseName;
	std::string basePath;
};


}
}

#endif /* TOMAHAWKOUTPUTSORT_H_ */

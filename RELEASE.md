# Release 0.4.0

## Breaking Changes
* All changes are breaking because of structural rechanges. No files generated prior to this release are supported.

## Major Features And Improvements
* Added large number of new FLAG fields to `two` files
* Reintroduced speed mode for calculations when contingency tables are not required

# Release 0.3.3

## Major Features And Improvements
* None

## Bug Fixes and Other Changes
* Bug fixes
   * Viewing sorted two files fixed for edge cases

# Release 0.3.2

## Breaking Changes
* Using old sorted files does not break functionality but have to be resorted

## Major Features And Improvements
* Sorting `two` files not correctly produces two indices used for fast queries
* Viewing sorted two files is significantly faster

## Bug Fixes and Other Changes
* Bug fixes
   * Sort merge (`tomahawk sort -M`) now produces the correct index

# Release 0.3.1

## Major Features And Improvements
* None

## Bug Fixes and Other Changes
* Bug fixes
   * Sort merge (`tomahawk sort -M`) now produces the correct output
   * `view` ABI command now correctly triggers `-h`/`-H` flag

# Release 0.3.0

## Breaking Changes
* All changes are breaking. There is no backwards compatibility from this release!

## Major Features And Improvements
* All major classes are now implemented as STL-like container and are as decoupled as possible
* External sort file handles now buffer a small amount of data to reduce random access lookups
* `two`/`twk` entries are no longer forced to be accessed by unaligned memory addresses
* Both `two` and `twk` are now self-indexing. All external indices are now invalid

## Bug Fixes and Other Changes
* Bug fixes
  * Too many to list here
* Examples updates:
  * Added R script to demonstrate simple plotting using `base`
  * Added several figures demonstrating some keys concepts of Tomahawk
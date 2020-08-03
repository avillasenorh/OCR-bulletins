# OCR-bulletins
OCR, Proofreading and Processing Scanned Seismological Bulletins

1. Contents of this repository

* `src`: software for processing and proof-reading OCR output from seismic bulletins
* `stations`: files with list of valid station names used in the ISS bulletins (1918-1963)
* `examples`: example files to test the processing codes


2. Contents of the `src` repository

* `fmt56`: C programs to proofread OCR output from ISS bulletins for the time period 1918-1956
* `fmt57`: C programs to proofread OCR output from ISS bulletins for the time period 1957-1963
* `baas`: C programs to reformat BAAS bulletins
* `gr`: C programs to reformat Gutenberg's notebooks
* `isc`: C programs to convert output of `fmt56` and `fmt57` to ISC's FFB format

3. Usage

Proofread and reformat txt files:

`$ fmt56 i53_0020.txt`
`$ fmt57 i59_0020.txt`



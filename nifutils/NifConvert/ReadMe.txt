NifConMerge forked from from NifUtils. Combines functionality of NifConvert and ChunkMerge
into one tool, and added batch processing.  This is NOT a replacement for either of those tools 
yet. A lot of functionality from chunkmerge was not ported in interests of quickly processing lots
of nifs with common options. 

Instead of specifying templates, you must have a template.nif for nifconverter in the input
directory. For collision templates, you must have a template_col.nif.

To use a copy of a converted nif for collision src, check the appropriate box. Otherwise, a
*_col.nif file will be sought for collision src. It can optionally be deleted at end of processing.

Conversion always occurs, adding collision mesh is optional.

Lost nif.xml functionality. Lost single file functionality. Texture functionality was not tested,
and is probably broken. 
TODO: Usage notes

Readme from Nifconverter:

--------------------------------------------------------------------------------------------
SOFTWARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.

AUTHOR DISCLAIMS ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, WITH
RESPECT TO THE PROGRAM. THE END-USER ASSUMES ALL RISK AS TO THE SUITABILITY, QUALITY,
AND PERFORMANCE OF THE PROGRAM. IN NO EVENT WILL AUTHOR, OR ITS DIRECTORS, OFFICERS,
EMPLOYEES OR AFFILIATES, BE LIABLE TO THE END-USER FOR ANY CONSEQUENTIAL, INCIDENTAL,
INDIRECT, SPECIAL OR EXEMPLARY DAMAGES (INCLUDING DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF DATA OR BUSINESS INFORMATION, AND THE LIKE) ARISING OUT
OF THE USE OF OR INABILITY TO USE THE PROGRAM OR ACCOMPANYING WRITTEN MATERIALS, EVEN
IF AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
--------------------------------------------------------------------------------------------


THANKS TO:

- neomonkeus from NifTools forum for his global support
- ttl269 from NifTools forum for his support decoding bhkCompressedMeshShpae
- Macoron3 from NifTools forum for his tipp creating collision data from other shapes than RootCollisionObject
- and many others :-)




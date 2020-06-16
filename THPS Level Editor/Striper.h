///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Source code for "Creating Efficient Triangle Strips"
// (C) 2000, Pierre Terdiman (p.terdiman@wanadoo.fr)
//
// Version is 2.0.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __STRIPER_H__
#define __STRIPER_H__

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//																Class Striper
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct STRIPERCREATE{
				STRIPERCREATE()
				{
					DFaces				= NULL;
					WFaces				= NULL;
					NbFaces				= 0;
					AskForWords			= true;
					OneSided			= true;
					SGIAlgorithm		= true;
					ConnectAllStrips	= false;
				}
				unsigned int					NbFaces;			// #faces in source topo
				unsigned int*					DFaces;				// list of faces (dwords) or null
				unsigned short*					WFaces;				// list of faces (words) or null
				bool					AskForWords;		// true => results are in words (else dwords)
				bool					OneSided;			// true => create one-sided strips
				bool					SGIAlgorithm;		// true => use the SGI algorithm, pick least connected faces first
				bool					ConnectAllStrips;	// true => create a single strip with void faces
	};

	struct STRIPERRESULT{
				unsigned int					NbStrips;			// #strips created
				unsigned int*					StripLengths;		// Lengths of the strips (NbStrips values)
				void*					StripRuns;			// The strips in words or dwords, depends on AskForWords
				bool					AskForWords;		// true => results are in words (else dwords)
	};

	class Striper
	{
	private:
				Striper&				FreeUsedRam();
				unsigned int					ComputeBestStrip(unsigned int face);
				unsigned int					TrackStrip(unsigned int face, unsigned int oldest, unsigned int middle, unsigned int* strip, unsigned int* faces, bool* tags);
				bool					ConnectAllStrips(STRIPERRESULT& result);

				Adjacencies*			mAdj;				// Adjacency structures
				bool*					mTags;				// Face markers

				unsigned int					mNbStrips;			// The number of strips created for the mesh
				CustomArray*			mStripLengths;		// Array to store strip lengths
				CustomArray*			mStripRuns;			// Array to store strip indices

				unsigned int					mTotalLength;		// The length of the single strip
				CustomArray*			mSingleStrip;		// Array to store the single strip

				// Flags
				bool					mAskForWords;
				bool					mOneSided;
				bool					mSGIAlgorithm;
				bool					mConnectAllStrips;

	public:
				Striper();
				~Striper();

				bool					Init(STRIPERCREATE& create);
				bool					Compute(STRIPERRESULT& result);
	};

#endif // __STRIPER_H__

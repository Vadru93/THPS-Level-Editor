void ProcessExtension(ChunkHeader Ext)
{
	Extpos = filepos
	while ((Extpos + Ext.size) > filepos)
	{
		read ChunkHeader 12 bytes
		switch (ChunkHeader.ID)
		{
		case 0x0000050E://Material Split - those splits are MOSTLY(see flags) stored with tristrip method so you need to convert it to trilist
			read flags 4 bytes//should be same for all pc maps, but may differ on xbox and ps2 maps(paris and oil)
			read numsplits 4 bytes
			read totallnumindices 4 bytes
			for each split
			{
			  read numindices
			  read materialindex//in th4+ it's material id could just make each index id so material[0].checksum = 0(tho idk maybe need index+1 since checksum==0 maybe will trash it)
			  read indices numindices * 4
	    	}
			break;
		case 0x0000011D://Collision PLG - axis aligned BSP Tree
			break;
		case 0x00000120://Materials Effect - never used?
			skip ChunkHeader.size
			break;
		case 0x0294AF01://Th3 extension - faceflags for collision
			break;
		case 0x1://struct
			skip ChunkHeader.size;
			break;
		}
	}
}

void ProcessTexture(ChunkHeader texture)
{
	texpos = filepos
	while (texpos + texture.size > filepos)
	{
		read ChunkHeader 12 bytes
	    case 0x2://texture name 1st appearence== 1st pass in th4+ 2nd appearance== alpha map(never used?)
		    if (1st appearance)
		   	  read texturename[256] ChunkHeader.size//NULL padded
		    else
		      read alphamapname[256] ChunkHeader.size//NULL paddded
		    break;
	    case 0x3://extension - will contain material flags, animations etc
		    ProcessMaterialExt(ChunkHeader)
			break;
	    default://struct(0x1) will contain texture flags, but if texture is in tdx those flags are overriden by tdx texflags
		    skip ChunkHeader.size;
		    break;
	}
}

void ProcessMaterial(ChunkHeader material)
{
	matpos = filepos
	while (matpos + material.size > filepos)
	{
		read ChunkHeader 12 bytes
	    case 0x6://texture
			ProcessTexture(ChunkHeader);
			break;
		default:
			skip ChunkHeader.size
		    break;
	}
}

void ProcessMaterials(ChunkHeader container)
{
	containerpos = filepos
    skip 12 bytes
	read nummaterials 4
	skip 4 bytes*nummaterials

	while (containerpos + container.size > filepos)
	{
		read ChunkHeader 12 bytes
		switch (ChunkHeader.ID)
		{
		case 0x7://material
			ProcessMaterial(ChunkHeader);
			break;
		default:
			skip ChunkHeader.size
		    break;
		}
	}
}

void LoadBSP()

{
	skip 72 bytes
    read WorldFlags 4 bytes
	while (filepos < filelength)
	{
		read ChunkHeader 12 bytes
		switch(ChunkHeader.ID)
		{
		    case 0x00000008://Material Container
				ProcessMaterials(ChunkHeader)
				break;
			case 0x00000009://atomic section - object
				currobjheader = ChunkHeader
				currobjpos = filepos
				skip 16 bytes
			    read numfaces 4 bytes;
				read numverts 4 bytes
				read bboxmin 12 bytes
				read bboxmax 12 bytes
				skip 8 bytes

				read verts numverts * 12
				if (WorldFlags & 0x10)
				  read normals numverts * 4;
				read colors numverts * 4
				read texcoords numverts * 8
				if (WorldFlags & 0x40 || WorldFlags & 0x80)
				  skip numverts * 8
				read collisionfaces numfaces * 8//2 bytes sound then 2 bytes per index
				break;
			case 0x00000003://extension
				if (filepos <= currobjpos + currobjheader.size)//atomic extension
				{
				  ProcessExtension(ChunkHeader);
				}
				else//plane extension
				{
				  skip ChunkHeader.size
				}
				break;
			case 0x00000006:
			case 0x0000000B:
			case 0x0000000A:
				break;
			default:
				skip ChunkHeader.size
					break;
		}
	}
}
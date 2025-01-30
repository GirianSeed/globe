[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

VCL (VU Command Line�̗��́j���g�p����*.vcl����*.vsm���쐬����
�X�N���v�g�T���v��

<�X�N���v�g�̉��>
*.vcl����*.vsm���쐬����X�N���v�g�T���v���ɂ͈ȉ��̂��̂�����܂��B

vertex_color:		�O��̃����[�X�ł́Aspot_light�T���v���ɂ�����
			.vsm code���쐬���邽�߂ɂ�-L�I�v�V�������g���K�v
			������܂����B����̃����[�X���炱�̃I�v�V������
			��菜�����Ƃ��\�ɂȂ�A.vsm code�̍쐬��������
			����悤�ɂȂ�܂����B

lambert:		�O��̃����[�X�ł́Aspot_light�T���v���ɂ�����
			.vsm code���쐬���邽�߂ɂ�-L�I�v�V�������g���K�v
			������܂����B����̃����[�X���炱�̃I�v�V������
			��菜�����Ƃ��\�ɂȂ�A.vsm code�̍쐬��������
			����悤�ɂȂ�܂����B

specular:		���݂̃o�[�W������vcl�ł́Avu1spotS.vcl �� 
			vu1comib.vcl�̃��[�v�̍œK���͎��s���܂��B�]���āA
			���j�A�œK���R�[�h�̂ݍ쐬����܂��B

emboss:			���݂̃o�[�W������vcl�ł́Avcl-loop�T���v���̃�
			�[�v�œK���͎��s���܂��B�]���āA���j�A�œK���R�[�h
			�̂ݍ쐬����܂��B

makeall:		�񋟂���Ă���S�Ẵ}�N���R�[�h���쐬���邽�߂ɂ�
			���Ȃ�̎��Ԃ�������܂��i�`�R�O���j�B

<�N�����@>

	% ./vertex_color  : vertex_color shading �T���v�����쐬���܂��B
	% ./lambert	  : lambert shading �T���v�����쐬���܂��B
	% ./specular	  : phone/blinn shading �T���v�����쐬���܂��B
	% ./emboss	  : emboss-bump �T���v�����쐬���܂��B
	% ./reflectionmap : reflection/refraction-mapping �T���v�����쐬
			    ���܂��B
	% ./shadowmap	  : shadow/lightmap �T���v�����쐬���܂��B
	% ./makeall	  : �S�ẴT���v�����쐬���܂��B

<�}�C�N���R�[�h�@�\����>

vu1basic*:	RTP(rotation/translation/perspective) projection,
		TPC(texture perspective correction), 3 directional lights + ambient
vu1cull*:	RTP,TPC,3 directional lights + ambient
vu1point*:	RTP,TPC,3 point lights + ambient
vu1spot*:	RTP,TPC,3 spot lights + ambient
vu1fog*:	RTP,TPC,fog,3 dir. lights + ambient
vu1anti*:	RTP,TPC,anti alias, 3 dir. lights +ambient

vu1emboss*:	RTP,TPC,emboss shift, 3 lights + ambient

vu1shadowTex:	creates a black and white Image which is used as a texture for shadowmapping 
vu1shadowSTQ*:	calculates the ST and Q values for the shadowmap reciever object.
vu1lightmapSTQ*:calculates the ST and Q values for the lightmap reciever object

vu1fisheye:	FishEye lens transformation, 3 dir. lights,
		can be used to create textures for specrical
		texture mapping
vu1reflectS:	calculates the ST and Q values for the
		reflecting object (static reflection mapping)
vu1reflectR:	calculates the ST and Q values for the
		reflecting object where the texture was
		made on runtime with the vu1fisheye micro
vu1refractS:	calculates the ST and Q values for the
		refracting object (static refraction mapping)
vu1refractR:	calculates the ST and Q values for the
		refracting object where the texture was
		made on runtime with the vu1fisheye micro

vu1colorSat:	like vu1basicClip but a color saturation is perfromed for the color clamp.
		e.g.: finalColor=(2.5,1.2,0.6)*255 -> (2.5/2.5,1.2/2.5,0.6/2.5)*255
vu1combiSo:	RTP,TPC,anti alias,fog, 3 dir.lights , 3 point lights + ambient

vu1noshade:     RTP,TPC,no lighting calculation (ouput color is vertex color);
vu1vertexCspec : RTP,TPC,3 directional light + ambient, phong-lighting but vertex color
                 is used as diffuse color
 
vu1clone:       RTP,TPC,3 directional lights + ambient, pre-multiplies the local world matrix
                with a matrix to move the object
                                                            

<�}�C�N���R�[�h�X�y�b�N�\>

micro name      | shading type    | back-face culling | volume clip | light type |   vrt/sec
---------------------------------------------------------------------------------------------
vu1basicVo        vertex color            -                   -       directional  15434496
vu1cullVo         vertex color            +                   +       directional  10027248
vu1pointVo        vertex color            +                   +       point         5105072
vu1spotVo         vertex color            +                   +       spot          4873064
vu1fogVo          vertex color            +                   +       directional   8315664
vu1antiVo         vertex color            +                   +       directional   6486592
vu1basicClip      vertex color            -                   +       directional  12224240
vu1colorSat       vertex color            -                   +       directional  10070088
vu1noshade        vertex color            -                   +        -           19383728
vu1clone          vertex color            -                   +       directional  12224240
vu1vertexCspec vertex color+spec          +                   +       directional   3432744
vu1skin           vertex color            -                   -       directional   3597216 
 
vu1basicLo        lambert                 -                   -       directional  13415864
vu1cullLo         lambert                 +                   +       directional   8499680
vu1pointLo        lambert                 +                   +       point         4234552
vu1spotLo         lambert                 +                   +       spot          4215120
vu1fogLo          lambert                 +                   +       directional   8103480
vu1antiLo         lambert                 +                   +       directional   6327552
 
vu1cullSo         phong                   +                   +       directional   3513048
vu1pointSo        phong                   +                   +       point         1804712
vu1spotSo         phong                   +                   +       spot          1844808
vu1combiSo        phong                   +                   +       dir.+point    1243480
vu1cullBlinno     blinn                   +                   +       directional   3266368
 
vu1embossDir      vertex color            -                   +       directional   6713392
vu1embossPoint    vertex color            -                   +       point         2921408
vu1embossSpot     vertex color            -                   +       spot          3394608
 
vu1shadowTex        -                     -                   -       -            32509736
vu1shadowSTQ        -                     -                   +       -            10857952
vu1shadowSTQCull    -                     +                   +       -             8793904
vu1lightmapSTQ      -                     -                   +       -            10843672
vu1lightmapSTQCull  -                     +                   +       -             8902936
 
vu1fisheye    vertex color                +                   +       directional   7989800
vu1reflectS         -                     -                   +       -             7860496
vu1reflectR         -                     -                   +       -             7852992
vu1refractS         -                     -                   +       -             7652848
vu1refractR         -                     -                   +       -             7645792         

<���l>

VCL version 1.28 ���g�p�B

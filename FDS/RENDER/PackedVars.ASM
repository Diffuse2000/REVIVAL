.686
.MMX
.XMM

_DATA   SEGMENT PARA PUBLIC 'DATA'
	public	_pfMaxLight
	public	_pf255
	public	_pfr256
	public	_pf65536
	public	_pf32768
	public	_pf256
	public	_pf128
	public	_pf16
	public	_pf1
	public	_pf0

align 16
	_pfMaxLight	dd		250.0, 250.0, 250.0, 250.0
	_pf255		dd		255.0, 255.0, 255.0, 255.0
	_pfr256		dd		0.00390625, 0.00390625, 0.00390625, 0.00390625
	_pf65536	dd		65536.0, 65536.0, 65536.0, 65536.0
	_pf32768	dd		32768.0, 32768.0, 32768.0, 32768.0
	_pf256		dd		256.0, 256.0, 256.0, 256.0
	_pf128		dd		128.0, 128.0, 128.0, 128.0
	_pf16		dd		16.0, 16.0, 16.0, 16.0
	_pf1		dd		1.0, 1.0, 1.0, 1.0
	_pf0		dd		0.0, 0.0, 0.0, 0.0
align 1
_DATA   ENDS

_TEXT   SEGMENT PARA PUBLIC 'CODE'
	ASSUME  cs:_TEXT, ds:_DATA



_TEXT   ENDS

END
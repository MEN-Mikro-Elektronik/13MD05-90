<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="page">
    <filename>index</filename>
    <title></title>
    <name>index</name>
  </compound>
  <compound kind="file">
    <name>m_errstr.c</name>
    <path>/opt/menlinux/LIBSRC/MDIS_API/</path>
    <filename>m__errstr_8c</filename>
    <member kind="function">
      <type>char *</type>
      <name>M_errstring</name>
      <anchor>a0</anchor>
      <arglist>(int32 errCode)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>M_errstringTs</name>
      <anchor>a1</anchor>
      <arglist>(int32 errCode, char *strBuf)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mdis_api.c</name>
    <path>/opt/menlinux/LIBSRC/MDIS_API/</path>
    <filename>mdis__api_8c</filename>
    <member kind="function">
      <type>MDIS_PATH</type>
      <name>M_open</name>
      <anchor>a4</anchor>
      <arglist>(const char *device)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>MDIS_CreateDevice</name>
      <anchor>a5</anchor>
      <arglist>(char *device, int rtMode, int persist)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>MDIS_RemoveDevice</name>
      <anchor>a6</anchor>
      <arglist>(char *device)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>MDIS_OpenBoard</name>
      <anchor>a7</anchor>
      <arglist>(char *device)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>MDIS_RemoveBoard</name>
      <anchor>a8</anchor>
      <arglist>(char *device)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_close</name>
      <anchor>a9</anchor>
      <arglist>(MDIS_PATH path)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_read</name>
      <anchor>a10</anchor>
      <arglist>(MDIS_PATH path, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_write</name>
      <anchor>a11</anchor>
      <arglist>(MDIS_PATH path, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_getstat</name>
      <anchor>a12</anchor>
      <arglist>(MDIS_PATH path, int32 code, int32 *dataP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_setstat</name>
      <anchor>a13</anchor>
      <arglist>(MDIS_PATH path, int32 code, INT32_OR_64 data)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_getblock</name>
      <anchor>a14</anchor>
      <arglist>(MDIS_PATH path, u_int8 *buffer, int32 length)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>M_setblock</name>
      <anchor>a15</anchor>
      <arglist>(MDIS_PATH path, const u_int8 *buffer, int32 length)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>M_ADDRSPACE</name>
    <filename>structM__ADDRSPACE.html</filename>
  </compound>
  <compound kind="struct">
    <name>M_SG_BLOCK</name>
    <filename>structM__SG__BLOCK.html</filename>
    <member kind="variable">
      <type>int32</type>
      <name>size</name>
      <anchor>o0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>data</name>
      <anchor>o1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>mdisapidummy</name>
    <title>MEN logo</title>
    <filename>mdisapidummy</filename>
  </compound>
</tagfile>

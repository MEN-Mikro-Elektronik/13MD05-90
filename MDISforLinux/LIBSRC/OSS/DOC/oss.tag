<?xml version='1.0' encoding='ISO-8859-1' standalone='yes'?>
<tagfile>
  <compound kind="page">
    <filename>index</filename>
    <title></title>
    <name>index</name>
  </compound>
  <compound kind="file">
    <name>oss.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss_8c</filename>
    <member kind="function">
      <type></type>
      <name>EXPORT_SYMBOL</name>
      <anchor>a7</anchor>
      <arglist>(OSS_Init)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Init</name>
      <anchor>a66</anchor>
      <arglist>(char *instName, OSS_HANDLE **ossP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Exit</name>
      <anchor>a67</anchor>
      <arglist>(OSS_HANDLE **ossP)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_DbgLevelSet</name>
      <anchor>a68</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 newLevel)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_DbgLevelGet</name>
      <anchor>a69</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss.h</name>
    <path>/opt/menlinux/INCLUDE/COM/MEN/</path>
    <filename>oss_8h</filename>
    <class kind="struct">OSS_CALLBACK_SET</class>
    <class kind="struct">OSS_DL_LIST</class>
    <class kind="struct">OSS_DL_NODE</class>
    <class kind="struct">OSS_RESOURCES</class>
    <class kind="struct">OSS_SHMEM_CLEAR</class>
    <class kind="struct">OSS_SHMEM_LINK</class>
    <class kind="struct">OSS_SHMEM_SET</class>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_8</name>
      <anchor>a30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_16</name>
      <anchor>a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_32</name>
      <anchor>a32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_DM_64</name>
      <anchor>a33</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_16UD</name>
      <anchor>a34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_24UD</name>
      <anchor>a35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_24UB</name>
      <anchor>a36</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UD</name>
      <anchor>a37</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UB</name>
      <anchor>a38</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OSS_VME_AM_32UB64</name>
      <anchor>a39</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a126</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a127</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a128</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemCopy</name>
      <anchor>a129</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, char *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemFill</name>
      <anchor>a130</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 size, char *adr, int8 value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_StrCpy</name>
      <anchor>a131</anchor>
      <arglist>(OSS_HANDLE *oss, char *from, char *to)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_StrLen</name>
      <anchor>a132</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *string)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrCmp</name>
      <anchor>a133</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *str1, char *str2)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrNcmp</name>
      <anchor>a134</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *str1, char *str2, u_int32 nbrOfBytes)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_StrTok</name>
      <anchor>a135</anchor>
      <arglist>(OSS_HANDLE *osHdl, char *string, char *separator, char **lastP)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqMask</name>
      <anchor>a136</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqUnMask</name>
      <anchor>a137</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IrqLevelToVector</name>
      <anchor>a138</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, int32 level, int32 *vector)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockCreate</name>
      <anchor>a139</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRemove</name>
      <anchor>a140</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockAcquire</name>
      <anchor>a141</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRelease</name>
      <anchor>a142</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigCreate</name>
      <anchor>a143</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 value, OSS_SIG_HANDLE **sigHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigSend</name>
      <anchor>a144</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE *sigHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigRemove</name>
      <anchor>a145</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE **sigHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigInfo</name>
      <anchor>a146</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SIG_HANDLE *sigHdl, int32 *signalNbrP, int32 *processIdP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a147</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 semType, int32 initVal, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a148</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a149</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE *semHandle, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a150</anchor>
      <arglist>(OSS_HANDLE *osHdl, OSS_SEM_HANDLE *semHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_DbgLevelSet</name>
      <anchor>a151</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 newLevel)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_DbgLevelGet</name>
      <anchor>a152</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a153</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a154</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a155</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a156</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a157</anchor>
      <arglist>(OSS_HANDLE *osHdl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a158</anchor>
      <arglist>(OSS_HANDLE *osHdl, u_int32 mikroSec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a159</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a160</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a161</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a162</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IsaGetConfig</name>
      <anchor>a163</anchor>
      <arglist>(OSS_HANDLE *osHdl, int8 *cardVendId, int16 cardProdId, int8 *devVendId, int16 devProdId, int32 devNbr, int32 resType, int32 resNbr, u_int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a164</anchor>
      <arglist>(OSS_HANDLE *osHdl, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a165</anchor>
      <arglist>(OSS_HANDLE *osHdl, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a166</anchor>
      <arglist>(OSS_HANDLE *osHdl, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Sprintf</name>
      <anchor>a167</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Vsprintf</name>
      <anchor>a168</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt, va_list arg)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmCreate</name>
      <anchor>a169</anchor>
      <arglist>(OSS_HANDLE *oss, void(*funct)(void *arg), void *arg, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmRemove</name>
      <anchor>a170</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmSet</name>
      <anchor>a171</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm, u_int32 msec, u_int32 cyclic, u_int32 *realMsec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmClear</name>
      <anchor>a172</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm)</arglist>
    </member>
    <member kind="function">
      <type>u_int16</type>
      <name>OSS_Swap16</name>
      <anchor>a190</anchor>
      <arglist>(u_int16 word)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap32</name>
      <anchor>a191</anchor>
      <arglist>(u_int32 dword)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_LIST *</type>
      <name>OSS_DL_NewList</name>
      <anchor>a193</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_Remove</name>
      <anchor>a194</anchor>
      <arglist>(OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_RemHead</name>
      <anchor>a195</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_AddTail</name>
      <anchor>a196</anchor>
      <arglist>(OSS_DL_LIST *l, OSS_DL_NODE *n)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_alarm.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__alarm_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmCreate</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, void(*funct)(void *arg), void *arg, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmRemove</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE **alarmP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmSet</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm, u_int32 msec, u_int32 cyclic, u_int32 *realMsecP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AlarmClear</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_HANDLE *alarm)</arglist>
    </member>
    <member kind="function">
      <type>OSS_ALARM_STATE</type>
      <name>OSS_AlarmMask</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_AlarmRestore</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_ALARM_STATE oldState)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_bustoaddr.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__bustoaddr_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 mergedBusNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, int32 mergedBusNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_clib.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__clib_8c</filename>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemCopy</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *src, char *dest)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_MemFill</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, char *adr, int8 value)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_StrLen</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, char *string)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrCmp</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_StrNcmp</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, char *str1, char *str2, u_int32 nbrOfBytes)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>OSS_StrTok</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, char *string, char *separator, char **lastP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Sprintf</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Vsprintf</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, char *str, const char *fmt, va_list arg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_StrCpy</name>
      <anchor>a8</anchor>
      <arglist>(OSS_HANDLE *oss, char *from, char *to)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_dl_list.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__dl__list_8c</filename>
    <member kind="function">
      <type>OSS_DL_LIST *</type>
      <name>OSS_DL_NewList</name>
      <anchor>a0</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_Remove</name>
      <anchor>a1</anchor>
      <arglist>(OSS_DL_NODE *n)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_RemHead</name>
      <anchor>a2</anchor>
      <arglist>(OSS_DL_LIST *l)</arglist>
    </member>
    <member kind="function">
      <type>OSS_DL_NODE *</type>
      <name>OSS_DL_AddTail</name>
      <anchor>a3</anchor>
      <arglist>(OSS_DL_LIST *l, OSS_DL_NODE *n)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_ident.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__ident_8c</filename>
  </compound>
  <compound kind="file">
    <name>oss_intern.h</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__intern_8h</filename>
    <class kind="struct">OSS_HANDLE</class>
  </compound>
  <compound kind="file">
    <name>oss_irq.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__irq_8c</filename>
    <member kind="function">
      <type>OSS_IRQ_STATE</type>
      <name>OSS_IrqMaskR</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqRestore</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle, OSS_IRQ_STATE oldState)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqMask</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OSS_IrqUnMask</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IrqLevelToVector</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 irqNbr, int32 *vectorP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_isa_pnp.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__isa__pnp_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_IsaGetConfig</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int8 *cardVendId, int16 cardProdId, int8 *devVendId, int16 devProdId, int32 devNbr, int32 resType, int32 resNbr, u_int32 *valueP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_map.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__map_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_mem.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__mem_8c</filename>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_resource.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__resource_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_sem.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__sem_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 semType, int32 initVal, OSS_SEM_HANDLE **semP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_sig.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__sig_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigCreate</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 signal, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigRemove</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE **sigP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigSend</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SigInfo</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SIG_HANDLE *sig, int32 *signalP, int32 *pidP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_spinlock.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__spinlock_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockCreate</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRemove</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockAcquire</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SpinLockRelease</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_swap.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__swap_8c</filename>
    <member kind="function">
      <type>u_int16</type>
      <name>OSS_Swap16</name>
      <anchor>a0</anchor>
      <arglist>(u_int16 word)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_Swap32</name>
      <anchor>a1</anchor>
      <arglist>(u_int32 dword)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_task.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__task_8c</filename>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>oss_time.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>oss__time_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 usec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a5</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu_8c</filename>
  </compound>
  <compound kind="file">
    <name>ossu_bustoaddr.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__bustoaddr_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_BusToPhysAddr</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, void **physicalAddrP,...)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciGetConfig</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 *valueP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSetConfig</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 pciDevNbr, int32 pciFunction, int32 which, int32 value)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_PciSlotToPciDevice</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busNbr, int32 mechSlot, int32 *pciDevNbrP)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_ident.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__ident_8c</filename>
  </compound>
  <compound kind="file">
    <name>ossu_map.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__map_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MapPhysToVirtAddr</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, void *physAddr, u_int32 size, int32 addrSpace, int32 busType, int32 busNbr, void **virtAddrP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_UnMapVirtAddr</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void **virtAddrP, u_int32 size, int32 addrSpace)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_mem.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__mem_8c</filename>
    <member kind="function">
      <type>void *</type>
      <name>OSS_MemGet</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 size, u_int32 *gotsizeP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemFree</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MemChk</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, void *addr, u_int32 size, int32 mode)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_resource.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__resource_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_AssignResources</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss, int32 busType, int32 busNbr, int32 resNbr, OSS_RESOURCES res[])</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_sem.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__sem_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemCreate</name>
      <anchor>a1</anchor>
      <arglist>(OSS_HANDLE *oss, int32 semType, int32 initVal, OSS_SEM_HANDLE **semP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemRemove</name>
      <anchor>a2</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE **semHandleP)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemWait</name>
      <anchor>a3</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_SemSignal</name>
      <anchor>a4</anchor>
      <arglist>(OSS_HANDLE *oss, OSS_SEM_HANDLE *sem)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_task.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__task_8c</filename>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_GetPid</name>
      <anchor>a0</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ossu_time.c</name>
    <path>/opt/menlinux/LIBSRC/OSS/</path>
    <filename>ossu__time_8c</filename>
    <member kind="function">
      <type>int32</type>
      <name>OSS_Delay</name>
      <anchor>a6</anchor>
      <arglist>(OSS_HANDLE *oss, int32 msec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelayInit</name>
      <anchor>a7</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_MikroDelay</name>
      <anchor>a8</anchor>
      <arglist>(OSS_HANDLE *oss, u_int32 usec)</arglist>
    </member>
    <member kind="function">
      <type>int32</type>
      <name>OSS_TickRateGet</name>
      <anchor>a9</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
    <member kind="function">
      <type>u_int32</type>
      <name>OSS_TickGet</name>
      <anchor>a10</anchor>
      <arglist>(OSS_HANDLE *oss)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>linossalarmusage</name>
    <title></title>
    <filename>linossalarmusage</filename>
  </compound>
  <compound kind="page">
    <name>linsemusage</name>
    <title></title>
    <filename>linsemusage</filename>
  </compound>
  <compound kind="page">
    <name>linsigusage</name>
    <title></title>
    <filename>linsigusage</filename>
  </compound>
</tagfile>

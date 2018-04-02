/*
 * @file zll-api.h
 *
 * @brief API for sending and receiving touchlink (formerly Zigbee Light Link) commissioning messages.
 *
 * <!--Copyright 2010-2018 by Silicon Laboratories, inc. All rights reserved.         *80*-->
 */

#ifndef SILABS_ZLL_API_H
#define SILABS_ZLL_API_H

/**
 * @addtogroup zll
 * See zll-api.h for source code.
 * @{
 */

/** @brief This will set the device type as a router or end device
 * (depending on the passed nodeType) and setup a distributed
 * commissioning network with the passed parameters.  If panId is 0xFFFF,
 * a random PAN ID will be generated.  If extendedPanId is set to all F's,
 * then a random extended pan ID will be generated.  If channel is 0xFF,
 * then channel 11 will be used.
 * If all F values are passed for PAN Id or Extended PAN ID then the
 * randomly generated values will be returned in the passed structure.
 *
 * @param networkInfo A pointer to an ::EmberZllNetwork struct indicating the
 *   network parameters to use when forming the network.  If random values are
 *   requested, the stack's randomly generated values will be returned in the
 *   structure.
 * @param radioTxPower the radio output power at which a node is to operate.
 *
 * @return An ::EmberStatus value indicating whether the operation
 *   succeeded, or why it failed.
 */
EmberStatus emberZllFormNetwork(EmberZllNetwork* networkInfo,
                                int8_t radioTxPower);

/** @brief This call will cause the device to send a NWK start or join to the
 *  target device and cause the remote AND local device to start operating
 *  on a network together.  If the local device is a factory new end device
 *  then it will send a touchlink NWK start to the target requesting that the
 *  target generate new network parameters, otherwise, if the local device
 *  is a router, then it will form a new touchlink network itself. If the device is
 *  not factory new then the local device will send a NWK join request
 *  using the current network parameters.
 *
 * @param targetNetworkInfo A pointer to an ::EmberZllNetwork structure that
 *   indicates the info about what device to send the NWK start/join
 *   request to.  This information must have previously been returned
 *   from a touchlink scan.
 *
 * @return An ::EmberStatus value indicating whether the operation
 *   succeeded, or why it failed.
 */
EmberStatus emberZllJoinTarget(const EmberZllNetwork* targetNetworkInfo);

/** @brief This call will update touchlink security token information. Unlike
 *    emberZllSetInitialSecurityState, this can be called while a network is
 *    already established.
 *
 * @param securityState The security configuration to be set.
 *
 * @return An ::EmberStatus value indicating whether the operation
 *   suceeded, or why it failed.
 */
EmberStatus emberZllSetSecurityStateWithoutKey(const EmberZllInitialSecurityState *securityState);

/** @brief This call will cause the device to setup the security information
 *    used in its network.  It must be called prior to forming, starting, or
 *    joining a network.
 *
 * @param networkKey is a pointer to an ::EmberKeyData structure containing the
 *   value for the network key.  If the value is set to all F's, then a random
 *   network key will be generated.
 * @param securityState The security configuration to be set.
 *
 * @return An ::EmberStatus value indicating whether the operation
 *   succeeded, or why it failed.
 */
EmberStatus emberZllSetInitialSecurityState(const EmberKeyData *networkKey,
                                            const EmberZllInitialSecurityState *securityState);

/**
 * @brief This call will initiate a touchlink network scan on all the specified
 *   channels.  Results will be returned in ::emberZllNetworkFoundHandler().
 *
 * @param channelMask indicating the range of channels to scan.
 * @param radioPowerForScan the radio output power used for the scan requests.
 * @param nodeType the the node type of the local device.
 *
 * @return An ::EmberStatus value indicating whether the operation
 *   succeeded, or why it failed.
 */
EmberStatus emberZllStartScan(uint32_t channelMask,
                              int8_t radioPowerForScan,
                              EmberNodeType nodeType);

/**
 * @brief This call will change the mode of the radio so that the receiver is
 * on when the device is idle. This permits a touchlink target to receive incoming
 * scan requests and an initiator to communicate with a target before the network is
 * established.  The idle mode will be set to 'Rx On' until either the duration
 * expires or is set to zero, or unless emberZllCancelRxOnWhenIdle is called.
 *
 * Note that, if the device joins or attempts to join a network during this period,
 * then the Stack will suspend the special touchlink radio idle mode, and the Stack's
 * radio idle mode will revert to the joined default appropriate to the node type.
 * If, however, the device leaves, or the join fails, then the touchlink radio
 * idle mode will be resumed, provided that the duration timer has not expired.
 *
 * @param durationMs The duration in milliseconds for which to set the idle mode to 'Rx On'.
 * A duration of 0xffffffff indicates an indefinite duration, so that the device
 * will always be able to respond to touchlink requests when not joined.
 *
 * @return An ::EmberStatus value indicating whether the operation succeeded or
 * why it failed. A return value of EMBER_INVALID_CALL indicates that the
 * special touchlink radio idle mode is active, but has been suspended by the Stack,
 * because the device has joined a network, or is in the process of joining.
 *
 */
EmberStatus emberZllSetRxOnWhenIdle(uint32_t durationMs);

/**
 * @brief This call will cancel the special touchlink radio idle mode immediately,
 * without reverting the Stack's radio idle mode to its original setting.
 *
 */
void emberZllCancelRxOnWhenIdle(void);

/**
 * @brief This call will set the default radio power mode directly (This is the
 * radio state when the device is not actively transmitting, and may be either
 * EMBER_RADIO_POWER_MODE_RX_ON or EMBER_RADIO_POWER_MODE_RX_OFF.)
 *
 * @param mode is the power mode to set.
 */
void emberZllSetRadioIdleMode(EmberRadioPowerMode mode);

/**
 * @brief This call is fired when a touchlink network scan finds a touchlinkable
 *   device, which may be joined on either a centralized or a distributed network,
 *   or may not be joined at all.
 *   The network information will be returned to the application for
 *   processing.
 *
 * @param networkInfo is a pointer to an ::EmberZllNetwork struct containing
 *   the Zigbee and touchlink specific information about the discovered network.
 * @param deviceInfo is a pointer to an ::EmberZllDeviceInfoRecord struct
 *   containing the device specific info.  This pointer may be NULL,
 *   indicating the device has either 0 sub-devices, or more than 1
 *   sub-devices.
 */
void emberZllNetworkFoundHandler(const EmberZllNetwork* networkInfo,
                                 const EmberZllDeviceInfoRecord* deviceInfo);

/**
 * @brief This call is fired when a touchlink network scan is complete.
 *
 * @param status An ::EmberStatus value indicating whether the operation
 * succeeded, or why it failed.  If the status is not ::EMBER_SUCCESS, the
 * application should not attempt to start or join a network returned via
 * ::emberZllNetworkFoundHandler.
 */
void emberZllScanCompleteHandler(EmberStatus status);

/**
 * @brief This call is fired when network and group addresses are assigned to
 * a remote mode in a network start or network join request.
 *
 * @param addressInfo is a pointer to an ::EmberZllAddressAssignment struct
 *   containing the address assignment information.
 */
void emberZllAddressAssignmentHandler(const EmberZllAddressAssignment* addressInfo);

/**
 * @brief This call is fired when the device is a target of a touch link.
 *
 * @param networkInfo is a pointer to an ::EmberZllNetwork struct containing
 *   the Zigbee and touchlink specific information about the initiator.
 */
void emberZllTouchLinkTargetHandler(const EmberZllNetwork *networkInfo);

/**
 * @brief This call reads the touchlink Stack data token.
 */
void emberZllGetTokenStackZllData(EmberTokTypeStackZllData *token);

/**
 * @brief This call reads the touchlink Stack security token.
 */
void emberZllGetTokenStackZllSecurity(EmberTokTypeStackZllSecurity *token);

/**
 * @brief This call reads both the touchlink Stack data and security tokens.
 */
void emberZllGetTokensStackZll(EmberTokTypeStackZllData *data,
                               EmberTokTypeStackZllSecurity *security);

/**
 * @brief This call sets the touchlink Stack data token.
 */
void emberZllSetTokenStackZllData(EmberTokTypeStackZllData *token);

/**
 * @brief This call returns whether or not the network is a touchlink network, that is,
 * a distributed network formed by touchlink commissioning, and supporting
 * touchlink-compliant address and group id assignment.
 */
bool emberIsZllNetwork(void);

/**
 * @brief This call will alter the touchlink data token to reflect the fact that the
 * network is non-touchlink-compliant.
 */
void emberZllSetNonZllNetwork(void);

/**
 * @brief This call will get the policy that enables or disables touchlink processing,
 * and to enable or disable stealing a non-factory-new device.
 */
EmberZllPolicy emberZllGetPolicy(void);

/**
 * @brief This call will set the policy to enable or disable touchlink processing,
 * and to enable or disable stealing a non-factory-new device.
 */
EmberStatus emberZllSetPolicy(EmberZllPolicy policy);

/**
 * @brief This call will set the current logical and physical channel.
 */
EmberStatus emberSetLogicalAndRadioChannel(uint8_t channel);

/**
 * @brief This call will get the current logical channel.
 */
uint8_t emberGetLogicalChannel(void);

/**
 * @brief This call will set the default node type for a factory new device.
 *
 * @param The node type to be set.
 */
void emberSetZllNodeType(EmberNodeType nodeType);

/**
 * @brief This call will set additional capability bits in the touchlink state.
 *
 * @param A mask with the bits to be set or cleared.
 */
void emberSetZllAdditionalState(uint16_t mask);

/**
 * @brief This function is called to indicate whether a touchlink network
 * operation is in progress, that is, a network start or a network join.
 */
bool emberZllOperationInProgress(void);

/**
 * @brief This function is called to indicate whether the special touchlink
 * radio idle mode is active, i.e. whether the radio receiver will remain on when
 * the device is not transmitting, when the device is not on a network.
 */
bool emberZllRxOnWhenIdleGetActive(void);

/** @} // END addtogroup
 */

#endif // SILABS_ZLL_API_H
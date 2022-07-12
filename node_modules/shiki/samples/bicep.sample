@description('Name of the eventhub namespace')
param eventHubNamespaceName string

@description('Name of the eventhub name')
param eventHubName string

@description('The service principal')
param principalId string

// Create an event hub namespace
resource eventHubNamespace 'Microsoft.EventHub/namespaces@2021-01-01-preview' = {
  name: eventHubNamespaceName
  location: resourceGroup().location
  sku: {
    name: 'Standard'
    tier: 'Standard'
    capacity: 1
  }
  properties: {
    zoneRedundant: true
  }
}

// Create an event hub inside the namespace
resource eventHub 'Microsoft.EventHub/namespaces/eventhubs@2021-01-01-preview' = {
  parent: eventHubNamespace
  name: eventHubName
  properties: {
    messageRetentionInDays: 7
    partitionCount: 1
  }
}

// give Azure Pipelines Service Principal permissions against the event hub 

var roleDefinitionAzureEventHubsDataOwner = subscriptionResourceId('Microsoft.Authorization/roleDefinitions', 'f526a384-b230-433a-b45c-95f59c4a2dec')

resource integrationTestEventHubReceiverNamespaceRoleAssignment 'Microsoft.Authorization/roleAssignments@2018-01-01-preview' = {
  name: guid(principalId, eventHub.id, roleDefinitionAzureEventHubsDataOwner)
  scope: eventHubNamespace
  properties: {
    roleDefinitionId: roleDefinitionAzureEventHubsDataOwner
    principalId: principalId
  }
}

// From https://dev.azure.com/johnnyreilly/blog-demos/_git/permissioning-azure-pipelines-bicep-role-assignments?path=/infra/main.bicep